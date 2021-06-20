/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 19:15:55 by besellem          #+#    #+#             */
/*   Updated: 2021/06/20 16:17:41 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_is_openable(char *path, int flag)
{
	int	fd;

	fd = open(path, flag);
	if (fd == -1)
		return (0);
	close(fd);
	return (1);
}

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

	if (!lst)
		return ;
	tmp = lst;
	while (tmp)
	{
		if (tmp->content)
			ft_printf("%s%c", tmp->content, sep);
		tmp = tmp->next;
	}
}

// return the nth node of the list
t_list	*ft_lstindex(t_list *lst, size_t index)
{
	t_list	*tmp;
	size_t	i;

	i = 0;
	tmp = lst;
	while (tmp)
	{
		if (i++ == index)
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

// DEBUG PURPOSE
void	ft_lstprint_cmd(t_list *lst)
{
	t_list	*tmp;
	t_cmd	*cmd;
	size_t	i;

	if (!lst)
		return ;
	ft_putendl(B_RED "# START");
	tmp = lst;
	while (tmp)
	{
		cmd = tmp->content;
		ft_printf(B_GREEN "stat[%.8b] args_len[%2d] ",
			cmd->status_flag, cmd->args_len);
		if (cmd->args)
		{
			ft_putstr("args: ");
			i = 0;
			while (cmd->args && cmd->args[i])
				ft_printf("[%s]", cmd->args[i++]);
		}
		else
			ft_putstr(B_BLUE "no args");
		ft_putendl("");
		tmp = tmp->next;
	}
	ft_putendl(B_RED "# END" CLR_COLOR);
}

int	ft_find_in_strs(char *s, const char **strs)
{
	size_t	i;

	if (!s || !strs || !*strs)
		return (-1);
	i = 0;
	while (strs[i])
	{
		if (0 == ft_strcmp(s, strs[i]))
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

char	*ft_strnclean(char *s, const char *charset, size_t end)
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

void	__ft_free_cmds__(void)
{
	const t_list	*tmp = singleton()->lst;
	t_cmd			*cmd;

	while (tmp)
	{
		cmd = tmp->content;
		ft_strsfree(cmd->args_len, cmd->args);
		tmp = tmp->next;
	}
	ft_lstclear(&singleton()->lst, free);
}

void	ft_free_exit(int code)
{
	if (singleton())
	{
		__ft_free_cmds__();
		ft_lstclear(&singleton()->lst, free);
		if (singleton()->prompt)
			ft_memdel((void **)(&singleton()->prompt));
		if (singleton()->cwd)
			ft_memdel((void **)(&singleton()->cwd));
		if (singleton()->env)
			ft_lstclear(&singleton()->env, free);
		if (singleton()->hist.path)
			ft_memdel((void **)(&singleton()->hist.path));
		close(singleton()->hist.fd);
		if (singleton()->option.fd != STDIN_FILENO)
			close(singleton()->option.fd);
		free(singleton());
	}
	if (code != -1)
		exit(code);
}

void	*ft_error(char *message, char *file, int line)
{
	ft_dprintf(STDERR_FILENO, B_RED "%s:%d: " CLR_COLOR "%s\n",
		file, line, message);
	ft_free_exit(EXEC_FAILURE);
	return (NULL);
}

void	ft_list_sort(t_list **begin_list, int (*cmp)())
{
	t_list	*list;
	void	*content;

	list = *begin_list;
	while (list && list->next)
	{
		if (cmp(list->content, list->next->content) > 0)
		{
			content = list->content;
			list->content = list->next->content;
			list->next->content = content;
			list = *begin_list;
		}
		else
			list = list->next;
	}
}

char	**ft_lst2strs(t_list **lst)
{
	size_t	tmp_len;
	char	**new;
	t_list	*tmp;
	size_t	i;

	if (!lst || !*lst)
		return (NULL);
	new = ft_calloc(ft_lstsize(*lst) + 1, sizeof(char *));
	if (!new)
		return (NULL);
	i = 0;
	tmp = *lst;
	while (tmp)
	{
		tmp_len = ft_strlen(tmp->content);
		new[i] = (char *)ft_calloc(tmp_len + 1, sizeof(char));
		if (!new[i])
			return (ft_strsfree(i, new));
		ft_memcpy(new[i++], tmp->content, tmp_len);
		tmp = tmp->next;
	}
	new[i] = NULL;
	return (new);
}

t_list	*search_env(char *tofind, t_list **env)
{
	t_list	*tmp;
	size_t	len_tofind;

	len_tofind = 0;
	while (tofind[len_tofind] && tofind[len_tofind] != '=')
		++len_tofind;
	tmp = *env;
	while (tmp)
	{
		if ((char *)tmp->content
			&& 0 == ft_strncmp((char *)tmp->content, tofind, len_tofind)
			&& (((char *)(tmp->content))[len_tofind] == '='
			|| ((char *)(tmp->content))[len_tofind] == '\0'))
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

char	*ft_getenv(const char *name)
{
	t_list	*tmp;
	int		idx;
	char	*ptr;

	if (!name)
		return (NULL);
	tmp = singleton()->env;
	while (tmp)
	{
		ptr = ft_strchr(tmp->content, '=');
		if (ptr)
		{
			idx = ft_stridx(tmp->content, "=");
			if (idx != NOT_FOUND && 0 == ft_strncmp(name, tmp->content, idx))
				return (ft_strdup(ptr + 1));
		}
		else
		{
			if (0 == ft_strcmp(name, tmp->content))
				return (ft_strdup(""));
		}
		tmp = tmp->next;
	}
	return (NULL);
}
