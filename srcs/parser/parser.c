/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
/*   Updated: 2021/06/09 17:56:07 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static struct s_redirections	g_limits[] = {
	{";", 1, FLG_EO_CMD}, {"|", 1, FLG_PIPE}, {">>", 2, FLG_APPEND},
	{">", 1, FLG_OUTPUT}, {"<<", 2, FLG_DINPUT}, {"<", 1, FLG_INPUT},
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
	{
		ft_dprintf(STDERR_FILENO, "%s:%d: Error\n", __FILE__, __LINE__);
		exit(1);
		return (NULL);
	}
	cmd->args = ft_lst2strs(args);
	ft_lstclear(args, free);
	cmd->args_len = ft_strslen(cmd->args);
	cmd->status_flag = status;
	return (cmd);
}

int	found_str_limit(char *s, size_t i, t_list **args)
{
	t_cmd	*new;
	size_t	k;

	// if (!s[i])
	// {
	// 	ft_lstadd_back(args, ft_lstnew(ft_substr(s, 0, i)));
	// 	new = new_cmd(FLG_EOL, args);
	// 	ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
	// 	return (0);
	// }
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

	ft_asprintf(&ptr, "%.*s%d%s", i - 1, *s, singleton()->last_return_value,
		*s + i + 1);
	*s = ptr;
	return (ft_nblen(singleton()->last_return_value) - 1);
}

size_t	get_env_var(char **s, size_t i)
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
		return ((size_t)ft_malloc_error(__FILE__, __LINE__));
	ptr = NULL;
	tmp = search_env(p, &singleton()->env);
	if (tmp)
		ptr = ft_strchr(tmp->content, '=');
	new = ft_calloc(ft_strlen(*s) + ft_strlen(ptr) - len_str + 1, sizeof(char));
	if (!new)
		return ((size_t)ft_malloc_error(__FILE__, __LINE__));
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
*******************************************************************************/


// TRANSFER TO .H FILE
# define SPEC_CHARS " \\$'\""

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
		while (s[i] && ft_incharset(SPACES, s[i]) && quotes.first == 0)
			++s;
		if (!s[i])
			break ;

		if (quotes.first == 0)
		{
			limit = found_str_limit(s, i, &args);
			// ft_printf("s[%s] i[%d] limit[%d]\n", s, i, limit);
			if (limit)
			{
				quotes2close(0, &quotes, RESET_FLAG);
				s += i + limit;
				i = 0;
				continue ;
			}
		}

		// ft_printf("%s:%d: [%.4b] args_size[%2lld] [%s]\n", __FILE__, __LINE__,
			// quotes.first, ft_lstsize(args), s + i);

		if ('\\' == s[i] && 0 == quotes.first)
		{
			// if (!quotes.d_quote)
				ft_strnclean(s + i, "\\", 1); // remove `\' (backslash) from `s'
			// PRINT_ERR("here")
			// ft_printf("TO CLOSE: [%d]\n", ft_incharset(SPEC_CHARS, s[i]));
			if (ft_incharset(SPEC_CHARS, s[i]))
				++i;
			// ft_printf("TO CLOSE: [%d]\n", ft_incharset(SPEC_CHARS, s[i]));
			// quotes2close(s[i], &quotes, SET_FLAG);
			// continue ; // does nothing
		}
		else
		{
			quotes2close(s[i], &quotes, SET_FLAG);
			if ((('"' == s[i]) || ('\'' == s[i])) && quotes.did_change)
			{
				// PRINT_ERR("here")
				ft_strnclean(s + i, QUOTES, 1); // remove ``'"`` from `s'
				quotes2close(s[i], &quotes, SET_FLAG);

				// if (quotes.did_change && ft_incharset(QUOTES, s[i]))
				// {
				// 	PRINT_ERR("NEW -- -- ")
				// 	ft_strnclean(s + i, QUOTES, 1);
				// 	ft_lstadd_back(&args, ft_lstnew(ft_strdup("")));
				// 	s += i;
				// 	i = 0;
				// 	continue ;
				// }
				// ft_printf("quotes.first[%d] quotes.changed[%d] s[%s] (s + i)[%s] i[%d]\n",
					// quotes.first, quotes.did_change, s, s + i, i);
				// if ((('"' == s[i]) && (('"' == s[i]) << DBL_BSHFT) & quotes.first)
				// 	|| (('\'' == s[i]) && (('\'' == s[i]) << SGL_BSHFT) & quotes.first))
				// 	ft_lstadd_back(&args, ft_lstnew(ft_strdup("")));
				if ((quotes.did_change && !ft_incharset(QUOTES, s[i]))
				|| (!quotes.did_change && !s[i]))
				// if (quotes.did_change)
				{
					// PRINT_ERR("NEW")
					// ft_printf("%s%d: s[%s]\n", __FILE__, __LINE__, s);
					ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
					s += i;
					i = 0;
					// continue ; // does nothing
				}
			}
			else if ((!quotes.first || (quotes.first & (1 << DBL_BSHFT)))
				&& '$' == s[i])
			{
				++i;
				i += get_env_var(&s, i);
			}
			else
				++i;
		}
		if ((ft_incharset(SPACES, s[i]) && quotes.first == 0))
		{
			ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
			s += i;
			i = 0;
		}
		// if ((('"' == s[i]) && (('"' == s[i]) << DBL_BSHFT) & quotes.first)
		// || (('\'' == s[i]) && (('\'' == s[i]) << SGL_BSHFT) & quotes.first))
		// 	ft_lstadd_back(&args, ft_lstnew(ft_strdup("")));
	}
	if (!s[i])
	{
		t_cmd	*new;

		if (i > 0 && s[i - 1])
			ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
		new = new_cmd(FLG_EOL, &args);
		ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
	}
	// ft_lstprint_cmd(singleton()->lst);
	// ft_printf("\n");
}

/*******************************************************************************
-------- END PARSER --------
*******************************************************************************/

/*
TESTS TO DO:

env -i ./minishell
env -i SHLVL=1 ./minishell

ls -la; echo bonjour >> out; ls | grep srcs ;
ls -la; ec"ho" bo"njo"ur >> out ;echo ' bonjour "" "a\" $LESS\toi'\"\' > out
ls -la; ec"ho" bo"njo"ur >> out ;echo " bonjour '' 'a\' $LESS\toi"\"\' > out
*/
