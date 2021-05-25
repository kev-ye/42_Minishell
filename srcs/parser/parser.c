/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2021/05/25 14:30:21 by kaye             ###   ########.fr       */
=======
/*   Updated: 2021/05/25 15:24:40 by besellem         ###   ########.fr       */
>>>>>>> besellem
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define COMPLETE_METACHARACTERS "|&;()<> \t"
#define METACHARACTERS "|;<> \t"

// void	ft_parse(char *s)
// {
// 	size_t	i;
// 	size_t	j;

// 	// ft_lstiter(singleton()->lst, free);		// free all `void *content' nodes
// 	ft_lstclear(&singleton()->lst, free);	// free the list containing all parsed commands
// 	quotes2close(0, RESET_FLAG);			// reinit the quote test
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
// 			quotes2close(s[j], SET_FLAG);	// add quote test on this char
// 			if ((!quotes2close(0, SET_FLAG) && found_str_limit(s, i, j)))
// 				break ;
// 			++j;
// 		}
// 		ft_putstr_fd("\n", STDIN_FILENO);
// 		i = j + 1;
// 	}
// 	// ft_lstprint_cmd(singleton()->lst);
// }


// RESET_FLAG is used to reset static variables in `quotes2close'
#define RESET_FLAG 1

// SET_FLAG used to set static variables normally in `quotes2close'
#define SET_FLAG 0

// STATUS_FLAG used to get the status of the static variables in `quotes2close'
#define STATUS_FLAG -1

int	quotes2close(unsigned char c, int status)
{
	static int	s_quote = 0;
	static int	d_quote = 0;
	int			last;

	last = (s_quote == 1) || (d_quote == 1);
	if (RESET_FLAG == status)
	{
		s_quote = 0;
		d_quote = 0;
	}
	else if (SET_FLAG == status)
	{
		if ('\'' == c && d_quote == 0)
			s_quote = (s_quote == 0);
		else if ('"' == c && s_quote == 0)
			d_quote = (d_quote == 0);
	}
	return (last != ((s_quote == 1) || (d_quote == 1)));
}


// int	quotes2close(unsigned char c, int status)
// {
// 	static int	s_quote = 0;
// 	static int	d_quote = 0;
// 	int			last;

// 	last = (s_quote == 1) || (d_quote == 1);
// 	if (RESET_FLAG == status)
// 	{
// 		s_quote = 0;
// 		d_quote = 0;
// 	}
// 	else if (SET_FLAG == status)
// 	{
// 		if ('\'' == c && d_quote == 0)
// 			s_quote = (s_quote == 0);
// 		else if ('"' == c && s_quote == 0)
// 			d_quote = (d_quote == 0);
// 	}
// 	return (last != ((s_quote == 1) || (d_quote == 1)));
// }

char	**ft_lst2strs(t_list *lst, const char *sep)
{
	const size_t	sep_len = ft_strlen(sep);
	char			**new;
	t_list			*tmp;
	size_t			i;

	if (!lst || !sep)
		return (NULL);
	new = ft_calloc(ft_lstsize(lst) + 1, sizeof(char *));
	if (!new)
		return (NULL);
	i = 0;
	tmp = lst;
	while (tmp)
	{
		new[i] = ft_calloc(ft_strlen(tmp->content) + sep_len + 1, sizeof(char));
		if (!new[i])
			return (ft_strsfree(i, new));
		ft_memcpy(new[i], tmp->content, ft_strlen(tmp->content));
		if (tmp->next)
			ft_memcpy(new[i], sep, sep_len);
		++i;
		tmp = tmp->next;
	}
	new[i] = NULL;
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
	// ft_lst2strs()
	cmd->args = ft_split(s, ' '); 		// ############ TEMPORARY ##############
	cmd->args_len = ft_strslen(cmd->args);
	cmd->status_flag = status;
	return (cmd);
}

int	found_str_limit(char *s, size_t i)
{
	// static - no use to redefine it at each call
	static struct s_redirections	limits[] = {
		{";", FLG_EO_CMD}, {"|", FLG_PIPE}, {">>", FLG_APPEND},
		{">", FLG_OUTPUT}, {"<", FLG_INPUT}, {NULL, 0}
	};
	t_cmd		*new;
	size_t		k;

	// if (!s[j + 1])
	// {
	// 	new = new_cmd(ft_substr(s, i, j + 1), FLG_EOL);
	// 	ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
	// 	return (1);
	// }
	k = 0;
	while (limits[k].redir)
	{
		if (ft_strncmp(s + i, limits[k].redir, ft_strlen(limits[k].redir)) == 0)
		{
			new = new_cmd(ft_substr(s, 0, i), limits[k].flag);
			ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
			return (ft_strlen(limits[k].redir));
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

size_t	get_env_var(char **s, size_t i)
{
	const int	len_str = len_variable(*s + i);
	t_list		*tmp;
	char		*p;
	char		*ptr;
	char		*new;

	// if (*(*s + i) == '?')	// case: echo $?
	// {
	// 	*s = ft_itoa(singleton()->last_return_value);
	// 	return (ft_nblen(singleton()->last_return_value) - 1);
	// }
	if (len_str < 0)
	{
		// ft_printf(B_RED "VAR WRONG FORMAT\n" CLR_COLOR, __LINE__, len_str, i);
		return (0);
	}
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

#define SPEC_CHARS " $'\""

void	ft_parse(char *s)
{
	// struct s_quotes	quotes;
	// t_list			*args;
	size_t			i;

	// ft_bzero(&quotes, sizeof(struct s_quotes));
	// args = NULL;
	ft_lstclear(&singleton()->lst, free);
	quotes2close(0, RESET_FLAG);
	i = 0;
	while (s[i])
	{
	
		// ft_printf("%s:%d: [%s]\n", __FILE__, __LINE__, s + i);

		if (s[i] == '\\')
		{
			ft_strnclean(s + i++, "\\", 1);	// remove `\' (backslash) from `s'
			if (ft_incharset(SPEC_CHARS, s[i]))
				++i;
			continue ;
		}
		else
		{
			// if ((('"' == s[i]) || ('\'' == s[i])) && quotes.first == 0)
			// {
			// 	quotes.first = (('"' == s[i]) << 0) | (('\'' == s[i]) << 1);
			// }

			quotes2close(s[i], SET_FLAG);
			if ((('"' == s[i]) || ('\'' == s[i])) && !quotes2close(0, STATUS_FLAG))
			{
				ft_strnclean(s + i, "'\"", 1); // remove ``'"`` from `s'
				continue ;
			}

			if (s[i] == '$')
			{
				/*
				** search in env variables to replace `$' by its value
				*/
				++i;
				i += get_env_var(&s, i);
			}
			else
				++i;
		}
		// if (!s[i] || (ft_incharset(" \t", s[i]) && !quotes2close(0, STATUS_FLAG)))
		// {
		// 	ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
		// 	while (s[i] && s[i] == ' ')
		// 		++i;
		// 	ft_printf(B_GREEN "content: %s\n" CLR_COLOR, ft_lstlast(args)->content);
		// 	// ft_strnclean(s + i, s[i], 1);
		// }
		int	limit = found_str_limit(s, i);
		if (limit)
		{
			s += i + limit;
			i = 0;
		}
	}
	if (!s[i])
	{
		t_cmd	*new = new_cmd(ft_substr(s, 0, i), FLG_EOL);
		ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
	}
	// ft_printf("FINAL: [%s]\n", s);
	ft_lstprint_cmd(singleton()->lst);
	ft_printf("\n");
}



// void	ft_parse(char *s)
// {
// 	// struct s_quotes	quotes;
// 	// t_list			*args;
// 	size_t			i;

// 	// ft_bzero(&quotes, sizeof(struct s_quotes));
// 	// args = NULL;
// 	ft_lstclear(&singleton()->lst, free);
// 	quotes2close(0, RESET_FLAG);
// 	i = 0;
// 	while (s[i])
// 	{
	
// 		// ft_printf("%s:%d: [%s]\n", __FILE__, __LINE__, s + i);

// 		if (s[i] == '\\')
// 		{
// 			ft_strnclean(s + i++, "\\", 1);	// remove `\' (backslash) from `s'
// 			if (ft_incharset(SPEC_CHARS, s[i]))
// 				++i;
// 			continue ;
// 		}
// 		else
// 		{
// 			// if ((('"' == s[i]) || ('\'' == s[i])) && quotes.first == 0)
// 			// {
// 			// 	quotes.first = (('"' == s[i]) << 0) | (('\'' == s[i]) << 1);
// 			// }

// 			quotes2close(s[i], SET_FLAG);
// 			if ((('"' == s[i]) || ('\'' == s[i])) && !quotes2close(0, STATUS_FLAG))
// 			{
// 				ft_strnclean(s + i, "'\"", 1); // remove ``'"`` from `s'
// 				continue ;
// 			}

// 			if (s[i] == '$')
// 			{
// 				/*
// 				** search in env variables to replace `$' by its value
// 				*/
// 				++i;
// 				i += get_env_var(&s, i);
// 			}
// 			else
// 				++i;
// 		}
// 		// if (!s[i] || (ft_incharset(" \t", s[i]) && !quotes2close(0, STATUS_FLAG)))
// 		// {
// 		// 	ft_lstadd_back(&args, ft_lstnew(ft_substr(s, 0, i)));
// 		// 	while (s[i] && s[i] == ' ')
// 		// 		++i;
// 		// 	ft_printf(B_GREEN "content: %s\n" CLR_COLOR, ft_lstlast(args)->content);
// 		// 	// ft_strnclean(s + i, s[i], 1);
// 		// }
// 		if (found_str_limit(s, 0, i))
// 		{
// 			s += i;
// 			i = 0;
// 		}
// 	}
// 	if (!s[i])
// 	{
// 		t_cmd	*new = new_cmd(ft_substr(s, 0, i), FLG_EOL);
// 		ft_lstadd_back(&singleton()->lst, ft_lstnew(new));
// 	}
// 	// ft_printf("FINAL: [%s]\n", s);
// 	// ft_lstprint_cmd(singleton()->lst);
// }


/*
TESTS TO DO:

echo \""''\"""bonjour"
"''"bonjour
"''"""bonjour

echo \""''\"""bonjour"

ls -la; echo bonjour >> out; ls | grep srcs ;
echo ' bonjour "" "a\" $LESS\toi'\"\'
echo " bonjour '' 'a\' $LESS\toi"\"\'
*/
