/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
/*   Updated: 2021/06/21 14:02:37 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static struct s_redirections	g_limits[] = {
	{";", 1, FLG_EO_CMD},
	{"|", 1, FLG_PIPE},
	{">>", 2, FLG_APPEND},
	{">", 1, FLG_OUTPUT},
	{"<<", 2, FLG_DINPUT},
	{"<", 1, FLG_INPUT},
	{NULL, 0, 0}
};

// RESET_FLAG is used to reset static variables in `quotes2close'
#define RESET_FLAG 1

// SET_FLAG used to set static variables normally in `quotes2close'
#define SET_FLAG 0

// STATUS_FLAG used to get the status of the static variables in `quotes2close'
#define STATUS_FLAG -1

#define DBL_BSHFT 0
#define SGL_BSHFT 1

/*
** But de la fonction:
**
** savoir si on a deja eu un quote
** savoir quel quote est deja passe
** savoir quel quote etait le premier
** savoir si un quote doit etre ferme
*/
int	quotes2close(unsigned char c, t_quotes *q, int status)
{
	if (RESET_FLAG == status)
	{
		ft_bzero(q, sizeof(t_quotes));
		return (0);
	}
	q->did_change = ((q->d_quote == 1) || (q->s_quote == 1));
	if (SET_FLAG == status)
	{
		if ((('"' == c) && (('"' == c) << DBL_BSHFT) & q->first)
			|| (('\'' == c) && (('\'' == c) << SGL_BSHFT) & q->first))
			q->first = 0;
		else if (q->d_quote == 0 && q->s_quote == 0)
			q->first = (('"' == c) << DBL_BSHFT) | (('\'' == c) << SGL_BSHFT);
		if (('\'' == c) && 0 == q->d_quote)
			q->s_quote = (0 == q->s_quote);
		else if (('"' == c) && 0 == q->s_quote)
			q->d_quote = (0 == q->d_quote);
	}
	q->did_change = q->did_change != ((q->d_quote == 1) || (q->s_quote == 1));
	return (q->first != 0);
}

t_cmd	*new_cmd(uint16_t status, t_list **args)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		return (ft_error(ERR_MALLOC, __FILE__, __LINE__));
	cmd->args = ft_lst2strs(args);
	ft_lstclear(args, free);
	cmd->args_len = ft_strslen((char **)cmd->args);
	cmd->status_flag = status;
	return (cmd);
}

int	found_str_limit(char *s, size_t i, t_list **args)
{
	t_cmd	*new;
	size_t	k;

	k = 0;
	while (g_limits[k].redir)
	{
		if (ft_strncmp(s + i, g_limits[k].redir, g_limits[k].len) == 0)
		{
			if (i > 0)
				ft_lstadd_back(args, ft_lstnew(ft_substr(s, 0, i)));
			new = new_cmd(g_limits[k].flag, args);
			ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
			return (g_limits[k].len);
		}
		++k;
	}
	return (0);
}

int	len_variable(char *s)
{
	int	i;

	i = 0;
	if (!s || !s[i])
		return (NOT_FOUND);
	if ((!ft_isalnum(s[i]) && s[i] != '_') || ft_isdigit(s[i]))
		return (NOT_FOUND);
	++i;
	while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
		++i;
	if (!s[i] || !(ft_isalnum(s[i]) || s[i] == '_'))
		return (i);
	else
		return (NOT_FOUND);
}

// $? case
size_t	get_var_last_return_case(char **s, size_t i)
{
	char	*ptr;

	ft_asprintf(&ptr, "%.*s%c%s", i - 1, *s, LRV_REPLACEMENT,
		*s + i + 1);
	*s = ptr;
	return (ft_nblen(LRV_REPLACEMENT) - 1);
}

static size_t	get_env_var(char **s, size_t i)
{
	const int	len_str = len_variable(*s + i);
	t_list		*tmp;
	char		*p;
	char		*ptr;
	char		*new;

	if (*(*s + i) == '?')
		return (get_var_last_return_case(s, i));
	if (NOT_FOUND == len_str)
		return (0);
	p = ft_substr(*s, (unsigned int)i, len_str);
	if (!p)
		return ((size_t)ft_error(ERR_MALLOC, __FILE__, __LINE__));
	ptr = NULL;
	tmp = search_env(p, &singleton()->env);
	if (tmp)
		ptr = ft_strchr(tmp->content, '=');
	new = ft_calloc(ft_strlen(*s) + ft_strlen(ptr) - len_str + 1, sizeof(char));
	if (!new)
		return ((size_t)ft_error(ERR_MALLOC, __FILE__, __LINE__));
	ft_memcpy(new, *s, i - 1);
	if (ptr && ptr + 1)
		ft_memcpy(new + i - 1, ptr + 1, ft_strlen(ptr) - 1);
	ft_strcat(new, *s + i + len_str);
	*s = new;
	if (ptr)
		return (ft_strlen(ptr + 1) - 1);
	else
		return (-1);
}

/*******************************************************************************
-------- PARSER --------
******************************************************************************/

////////////////////////
// FOR DEBUGGING PURPOSE
#define __DEBUG__	FALSE
////////////////////////

// TRANSFER TO .H FILE
#define SPEC_CHARS "\\$'\""		/* Special chars n1 */
#define SC2 "\\$\""				/* Special chars for `\' in double quotes */

void	ft_parse(char *s)
{
	t_quotes	quotes;
	t_list		*args;
	size_t		i;
	int			limit;

	args = NULL;
	ft_lstclear(&singleton()->lst, free);
	quotes2close(0, &quotes, RESET_FLAG);
	i = 0;
	while (s[i])
	{
		while (s[i] && ft_incharset(SPACES, s[i]) && FALSE == quotes.first)
			++s;
		if (!s[i])
			break ;

		if (FALSE == quotes.first)
		{
			limit = found_str_limit(s, i, &args);
			if (limit)
			{
				quotes2close(0, &quotes, RESET_FLAG);
				s += i + limit;
				i = 0;
				continue ;
			}
		}

		if (__DEBUG__)
		{
			ft_printf(B_YELLOW "%s:%d:" CLR_COLOR " q.first[%.4b] ",
				__FILE__, __LINE__, quotes.first);
			ft_printf("s.sq[%s%d" CLR_COLOR "] ",
				(quotes.s_quote != 0 ? B_RED : ""), quotes.s_quote);
			ft_printf("s.dq[%s%d" CLR_COLOR "] ",
				(quotes.d_quote != 0 ? B_RED : ""), quotes.d_quote);
			ft_printf("args_size[%2d] [%s]\n", ft_lstsize(args), s + i);
		}

		if ('\\' == s[i])
		{
			if ((quotes.first && quotes.d_quote && ft_incharset(SC2, s[i + 1]))
				|| FALSE == quotes.first)
			{
				ft_strnclean(s + i, "\\", 1);
			}
			if (ft_incharset(SPEC_CHARS, s[i]))
				++i;
		}
		else
		{
			quotes2close(s[i], &quotes, SET_FLAG);
			if ((('"' == s[i]) || ('\'' == s[i])) && quotes.did_change)
			{
				
				if (__DEBUG__)
				{
					ft_printf(B_BLUE "q.first[%.4b] q.changed[%d] q.s[%d] q.d[%d]\n" CLR_COLOR,
						quotes.first, quotes.did_change, quotes.s_quote, quotes.d_quote);
				}

				ft_strnclean(s + i, QUOTES, 1); // remove ``'"`` from `s'
				
				if (!ft_incharset(QUOTES, s[i]))
					quotes2close(s[i], &quotes, SET_FLAG);

				if (__DEBUG__)
				{
					ft_printf(B_BLUE "q.first[%.4b] q.changed[%d] q.s[%d] q.d[%d]\n" CLR_COLOR,
						quotes.first, quotes.did_change, quotes.s_quote, quotes.d_quote);
				}

				if ((quotes.did_change && !ft_incharset(QUOTES, s[i]))
					|| (!quotes.did_change && !s[i]))
				{
					ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
					s += i;
					i = 0;
				}
			}
			else if ((FALSE == quotes.first || quotes.d_quote) && '$' == s[i])
			{
				if (FALSE == quotes.first && ft_incharset(QUOTES, s[i + 1]))
					ft_strnclean(s + i, "$", 1);
				else if (!singleton()->lst || !(((t_cmd *)ft_lstlast(singleton()->lst)->content)->status_flag & FLG_DINPUT))
				{
					++i;
					i += get_env_var(&s, i);
				}
				else
					++i;
			}
			else
				++i;
		}
		if (ft_incharset(SPACES, s[i]) && FALSE == quotes.first)
		{
			ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
			s += i;
			i = 0;
		}
	}
	if (!s[i])
	{
		t_cmd	*new;

		if (i > 0 && s[i - 1])
			ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
		new = new_cmd(FLG_EOL, &args);
		ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
	}

	if (__DEBUG__)
	{
		ft_lstprint_cmd(singleton()->lst);
		ft_printf("\n");
	}
}

/*******************************************************************************
-------- END PARSER --------
******************************************************************************/

/*
TESTS TO DO:

env -i ./minishell
env -i SHLVL=1 ./minishell

ls -la; echo bonjour >> out; ls | grep srcs ;
ls -la; ec"ho" bo"njo"ur >> out ;echo ' bonjour "" "a\" $LESS\toi'\"\' > out
ls -la; ec"ho" bo"njo"ur >> out ;echo " bonjour '' 'a\' $LESS\toi"\"\' > out
*/
