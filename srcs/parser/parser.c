/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:02:00 by besellem          #+#    #+#             */
/*   Updated: 2021/05/23 18:05:28 by besellem         ###   ########.fr       */
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
	static t_quotes	quotes = {.sgl = 0, .dbl = 0};

	if (reinit)
		ft_bzero(&quotes, sizeof(t_quotes));
	quotes.sgl += (c == '\'');
	quotes.dbl += (c == '"');
	return ((quotes.sgl % 2 == 1) || (quotes.dbl % 2 == 1));
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

// static int	found_str_limit(char *s, size_t i, size_t j)
// {
// 	quotes2close(s[j], 0);
// 	if (!quotes2close(0, 0) && s[j] == ';')
// 	{
// 		ft_lstadd_back(&singleton()->lst, ft_lstnew(ft_substr(s, i, j - i)));
// 	}
// 	else if (!quotes2close(0, 0) && s[j] == '|')
// 	{
// 		ft_lstadd_back(&singleton()->lst, ft_lstnew(ft_substr(s, i, j - i)));
// 	}
// 	else if (!quotes2close(0, 0) && !s[j + 1])
// 	{
// 		ft_lstadd_back(&singleton()->lst, ft_lstnew(ft_substr(s, i, j + 1)));
// 	}
// 	else
// 		return (0);
// 	return (1);
// }

static int	found_str_limit(char *s, size_t i, size_t j)
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

void	ft_parse(char *s)
{
	size_t	i;
	size_t	j;

	// ft_lstiter(singleton()->lst, free);		// free all `void *content' nodes
	ft_lstclear(&singleton()->lst, free);	// free the list containing all parsed commands
	quotes2close(0, REINIT_FLAG);			// reinit the quote test
	i = 0;
	while (s[i])
	{
		j = i;
		while (s[j])
		{
			quotes2close(s[j], NTHG_FLAG);	// add quote test on this char
			if ((!quotes2close(0, NTHG_FLAG) && found_str_limit(s, i, j)))
				break ;
			++j;
		}
		i = j + 1;
	}
	ft_lstprint_cmd(singleton()->lst, '\n');
}
