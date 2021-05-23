/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 19:15:55 by besellem          #+#    #+#             */
/*   Updated: 2021/05/23 17:52:04 by kaye             ###   ########.fr       */
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