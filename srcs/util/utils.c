/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 19:15:55 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 14:24:44 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_printstrs(int fd, char **strs)
{
	size_t	i;

	i = 0;
	while (strs[i])
		ft_putendl_fd(strs[i++], fd);
	ft_putstr_fd("\n", fd);
}

void	ft_lstprint(t_list *lst, char sep)
{
	t_list	*tmp;

	tmp = lst;
	while (tmp)
	{
		ft_printf("%s%c", tmp->content, sep);
		tmp = tmp->next;
	}
}

void	ft_lstprint_cmd(t_list *lst, char sep)
{
	t_list	*tmp;
	t_cmd	*cmd;
	size_t	i;

	tmp = lst;
	while (tmp)
	{
		ft_putendl("####");
		cmd = tmp->content;
		i = 0;
		while (cmd->args[i])
			ft_printf("arg[%s] stat[%.16b]%c", cmd->args[i++], cmd->status_flag, sep);
		ft_putstr("\n");
		tmp = tmp->next;
	}
}

int		ft_find_in_strs(char *s, const char **strs)
{
	size_t	i;

	if (!s || !strs || !*strs)
		return (-1);
	i = 0;
	while (strs[i])
	{
		if (ft_strcmp(s, strs[i]) == 0)
			return (i);
		++i;
	}
	return (-1);
}

/*
** Clean characters from `charset' found in `s'
** /!\ it modifies `s'
**
** ex:
** ft_strclean(";bonjour >to<i", ";<>");
** => "bonjour toi"
*/
char	*ft_strclean(char *s, const char *charset)
{
	size_t	i;
	size_t	j;

	if (!s || !charset)
		return (s);
	j = 0;
	i = 0;
	while (s[i])
	{
		if (!ft_incharset(charset, s[i]))
			s[j++] = s[i];
		++i;
	}
	ft_bzero(s + j, ft_strlen(s + j));
	return (s);
}

char		*ft_strnclean(char *s, const char *charset, size_t end)
{
	size_t	i;
	size_t	j;

	if (!s || !charset)
		return (s);
	j = 0;
	i = 0;
	while (s[i])
	{
		if (i >= end || !ft_incharset(charset, s[i]))
			s[j++] = s[i];
		++i;
	}
	ft_bzero(s + j, ft_strlen(s + j));
	return (s);
}

void	ft_free_exit(void)
{
	ft_lstclear(&singleton()->lst, free);
	free(singleton());
	exit(0);
}
