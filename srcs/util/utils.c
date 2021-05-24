/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 19:15:55 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 15:04:17 by kaye             ###   ########.fr       */
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

void	ft_free_exit(void)
{
	ft_lstclear(&singleton()->lst, free);
	free(singleton());
	exit(0);
}

void ft_list_sort(t_list **begin_list, int (*cmp)())
{
    t_list *list;
    void *content;

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

t_list	*ft_lstnew_env(void *content)
{
	t_list	*new;
	char *env;

	new = (t_list *)malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	env = ft_strdup((char *)content);
	if (!env)
		return (NULL);
	new->content = (void *)env;
	new->next = NULL;
	return (new);
}

t_list  *search_env(char *tofind, t_list **env)
{
    t_list *tmp;
    size_t len_tofind;

    len_tofind = 0;
    while (tofind[len_tofind] && tofind[len_tofind] != '=')
        ++len_tofind;
    tmp = *env;
    while (tmp)
    {
        if ((char *)tmp->content
            && !ft_strncmp((char *)tmp->content, tofind, len_tofind)
            && ((char *)(tmp->content))[len_tofind] == '=')
            return (tmp);
        else if ((char *)tmp->content
            && !ft_strncmp((char *)tmp->content, tofind, len_tofind)
            && ((char *)(tmp->content))[len_tofind] == '\0')
            return (tmp);
        tmp = tmp->next;
    }
    return (NULL);
}