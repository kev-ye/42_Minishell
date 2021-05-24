/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 19:00:20 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define COMPLETE_METACHARACTERS "|&;()<> \t"
#define METACHARACTERS "|;<> \t"
#define CTRL_OPERATORS "\n"

int	got_metacharacter(void);

// REINIT_FLAG is used to reinit static variables in `quotes2close' function
#define REINIT_FLAG 1

// NTHG_FLAG used to reinit static variables in `quotes2close' function
#define NTHG_FLAG 0

int	quotes2close(char c, int reinit)
{
	static t_quotes	quotes = {.sgl = 0, .dbl = 0, .first = 0};

	if (reinit)
		ft_bzero(&quotes, sizeof(t_quotes));
	if (c == '\'')
		quotes.sgl = (quotes.sgl == 0);
	else if (c == '"')
		quotes.dbl = (quotes.dbl == 0);

	if (quotes.sgl && !quotes.dbl)
	{
		quotes.first = QUOTE_FLG_SINGLE;
		// return (quotes.sgl);
	}
	else if (!quotes.sgl && quotes.dbl)
	{
		quotes.first = QUOTE_FLG_DOUBLE;
		// return (quotes.dbl);
	}

	return (quotes.first & QUOTE_FLG_SINGLE);

	printf("HERE\n");
	// return ((quotes.sgl == 0) || (quotes.dbl == 0));
}

int	cmd_args_len(char *cmd)
{
	int				len;
	t_quotes		quotes;
	size_t			i;

	ft_bzero(&quotes, sizeof(t_quotes));
	len = 0;
	i = 0;
	while (cmd[i])
	{
		quotes.sgl += (cmd[i] == '\'');
		quotes.dbl += (cmd[i] == '"');
		if ((quotes.sgl % 2) && (quotes.dbl % 2)
			&& (cmd[i] == ';' || cmd[i] == '|'))
			++len;
		++i;
	}
	return (len);
}

int		is_eol(char *s)
{
	return (*s == ';' || *s == '|' || *s == '>' || *s == '<'
			|| ft_strncmp(s, ">>", 2) == 0);
}

char	**clean_cmd(char *cmd)
{
	const int	len_ = cmd_args_len(cmd);
	char		**new;
	size_t		i;
	size_t		j;

	if (!cmd)
		return (NULL);
	new = ft_calloc(len_ + 1, sizeof(char *));
	if (!new)
		return (NULL);
	i = 0;
	j = 0;
	while (cmd[i])
	{
		while (is_eol(cmd + i))
			++i;
		new[j] = ft_calloc(1, sizeof(char));
		++i;
	}
	return (new);
}

t_cmd	*new_cmd(char *s, uint16_t status)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
	{
		ft_dprintf(STDERR_FILENO, "%s:%d: Error\n", __FILE__, __LINE__);
		exit(1);
		return (NULL);
	}
	// cmd->args = clean_cmd(s);
	cmd->args = ft_split(s, ' '); 		// ############ TEMPORARY ##############
	cmd->args_len = ft_strslen(cmd->args);
	cmd->status_flag = status;
	return (cmd);
}

int	found_str_limit(char *s, size_t i, size_t j)
{
	// static - no use to redefine it at each call
	static struct s_redirections	limits[] = {
		{";", FLG_EO_CMD}, {"|", FLG_PIPE}, {">>", FLG_APPEND},
		{">", FLG_OUTPUT}, {"<", FLG_INPUT}, {NULL, 0U}
	};
	t_cmd		*new;
	size_t		k;

	if (!s[j + 1])
	{
		new = new_cmd(ft_substr(s, i, j + 1), FLG_EOL);
		ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
		return (1);
	}
	k = 0;
	while (limits[k].redir)
	{
		if (ft_strncmp(s + j, limits[k].redir, ft_strlen(limits[k].redir)) == 0)
		{
			new = new_cmd(ft_substr(s, i, j - i), limits[k].flag);
			ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
			return (1);
		}
		++k;
	}
	return (0);
}

// void	ft_parse(char *s)
// {
// 	size_t	i;
// 	size_t	j;

// 	// ft_lstiter(singleton()->lst, free);		// free all `void *content' nodes
// 	ft_lstclear(&singleton()->lst, free);	// free the list containing all parsed commands
// 	quotes2close(0, REINIT_FLAG);			// reinit the quote test
// 	i = 0;
// 	while (s[i])
// 	{
// 		j = i;
// 		while (s[j])
// 		{
// 			ft_dprintf(STDIN_FILENO, "[%c]", s[j]);
// 			// if (s[j] == '\\' && ft_incharset(" $'\"", s[j + 1]))
// 			// {
// 			// 	// s[j] = ' '; // replace `\' (backslash) by a space
// 			// 	j += 2;
// 			// }
// 			quotes2close(s[j], NTHG_FLAG);	// add quote test on this char
// 			if ((!quotes2close(0, NTHG_FLAG) && found_str_limit(s, i, j)))
// 				break ;
// 			++j;
// 		}
// 		ft_putstr_fd("\n", STDIN_FILENO);
// 		i = j + 1;
// 	}
// 	// ft_lstprint_cmd(singleton()->lst, '\n');
// }

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
	if (!s[i] || ft_incharset(PARSER_LIMITS_CHARS, s[i]))
		return (i);
	else
		return (NOT_FOUND);
}


// __attribute__((noreturn));
void	*ft_malloc_error(char *file, int line)
{
	ft_dprintf(STDERR_FILENO, B_GREEN "%s:%d: Malloc Error\n" CLR_COLOR,
				file, line);
	ft_free_exit();
	return (NULL);
}

char	*get_env_var(char **s, size_t *i)
{
	const int	len_str = len_variable(*s + *i);
	t_list		*tmp;
	char		*p;
	char		*ptr;
	char		*new;

	if (len_str < 0)
		return (NULL);
	p = ft_substr(*s, (unsigned int)*i, len_str);
	if (!p)
		return (ft_malloc_error(__FILE__, __LINE__));
	ptr = NULL;
	tmp = search_env(p, &singleton()->env);
	if (tmp)
		ptr = ft_strchr(tmp->content, '=');
	new = ft_calloc(ft_strlen(*s) + ft_strlen(ptr) - len_str + 1, sizeof(char));
	if (!new)
		return (ft_malloc_error(__FILE__, __LINE__));
	ft_memcpy(new, *s, *i - 1);
	if (ptr)
		ft_memcpy(new + *i - 1, ptr + 1, ft_strlen(ptr) - 1);
	ft_strcat(new, *s + *i + len_str);
	*i = *i + ft_strlen(ptr) - 1;
	*s = new;
	return (*s);
}

#define SPEC_CHARS " $'\""

void	ft_parse(char *s)
{
	// char	*tmp;
	size_t	i;

	ft_lstclear(&singleton()->lst, free);
	quotes2close(0, REINIT_FLAG);
	i = 0;
	while (s[i])
	{
	
		// ft_printf("%s:%d: [%s]\n", __FILE__, __LINE__, s + i);

		if (s[i] == '\\')
		{
			ft_strnclean(s + i++, "\\", 1);	// remove `\' (backslash) from `s'
			// printf(" [%s]\n", s);
			if (ft_incharset(SPEC_CHARS, s[i]))
				++i;
			continue ;
		}
		else
		{
			quotes2close(s[i], NTHG_FLAG);
			if (s[i] == '$')
			{
				/*
				** search in env variables to replace `$' by its value
				*/
				++i;
				get_env_var(&s, &i);
				// printf("s[%s] i[%zu] s+i[%s]\n", s, i, s + i);
			}
			if (ft_incharset(SPEC_CHARS, s[i]))
			{
				// ft_strnclean(s + i, "\\", 1);
				// printf("after : [%s]\n", s);
			}
		}
		++i;

		// ft_printf("%s:%d: [%s]\n", __FILE__, __LINE__, s);
		// ft_printf("\n");

		// j = i;
		// while (s[j])
		// {
		// 	ft_dprintf(STDIN_FILENO, "[%c]", s[j]);
		// 	// if (s[j] == '\\' && ft_incharset(" $'\"", s[j + 1]))
		// 	// {
		// 	// 	// s[j] = ' '; // replace `\' (backslash) by a space
		// 	// 	j += 2;
		// 	// }
		// 	quotes2close(s[j], NTHG_FLAG);	// add quote test on this char
		// 	if ((!quotes2close(0, NTHG_FLAG) && found_str_limit(s, i, j)))
		// 		break ;
		// 	++j;
		// }
		// ft_putstr_fd("\n", STDIN_FILENO);
		// i = j + 1;
	}
	printf("[%s]\n", s);
	// ft_lstprint_cmd(singleton()->lst, '\n');
}


// echo \""''\"""bonjour"
// "''"bonjour
// "''"""bonjour

// echo \""''\"""bonjour"

/*





*/