/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:47 by kaye              #+#    #+#             */
/*   Updated: 2021/05/24 15:04:09 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char *head_env(char *env)
{
    char *env_head;
    int len;

    len = 0;
    env_head = NULL;
    while (env[len] && env[len] != '=')
        ++len;
    if (env[len] && env[len] == '=')
    {
        env_head = malloc(sizeof(char) * len + 1);
        if (!env_head)
            return (NULL);
        ft_strncpy(env_head, env, len + 1);
    }
    else
    {
        env_head = malloc(sizeof(char) * len);
        if (!env_head)
            return (NULL);
        ft_strncpy(env_head, env, len);
    }
    return (env_head);
}

static void add_quote(t_list **lst_env)
{
    t_list *tmp;
    char *new_env;
    char *tmp_env;
    char *env_head;

    tmp = *lst_env;
    env_head = NULL;
    while (tmp)
    {
        tmp_env = ft_strchr((char *)tmp->content, '=');
        if (tmp_env != NULL)
        {
            env_head = head_env((char *)tmp->content);
            if (!env_head)
                return ;
            ft_asprintf(&new_env, "declare -x %s\"%s\"", env_head, tmp_env + 1);
            if (!new_env && ft_memdel((void **)(&env_head)) == NULL)
                return ;
            free(env_head);
            free(tmp->content);
            tmp->content = new_env;
        }
        else
        {
            env_head = head_env((char *)tmp->content);
            if (!env_head)
                return ;
            ft_asprintf(&new_env, "declare -x %s", env_head);
            if (!new_env && ft_memdel((void **)(&env_head)) == NULL)
                return ;
            free(env_head);
            free(tmp->content);
            tmp->content = new_env;
        }
        tmp = tmp->next;
    }
}

static void new_shell_env(char *new_env, t_list *to_change, int to_add)
{
    if (to_change)
    {
        free(to_change->content);
        to_change->content = (char *)new_env;
    }
    if (to_add == 1)
        ft_lstadd_back(&singleton()->env, ft_lstnew((void *)new_env));
}

static void add_env(char **cmds)
{
    t_list *to_change;
    char *new_env;
    int i;

    i = 1;
    while (cmds[i])
    {
        to_change = search_env(cmds[i], &singleton()->env);
        new_env = ft_strdup(cmds[i]);
        if (!new_env)
            return ;
        if (to_change)
        {
            new_shell_env(new_env, to_change, 0);
            return ;
        }
        else
            new_shell_env(new_env, NULL, 1);
        ++i;
    }
}

static t_list *env_export(t_list **lst_env)
{
    t_list  *tmp;
    t_list  *new_lst_env;
    char    *env_content;

    tmp = *lst_env;
    while(tmp)
    {
        env_content = ft_strdup((char *)tmp->content);
        if (!env_content)
            return (NULL);
        ft_lstadd_back(&new_lst_env, ft_lstnew((void *)env_content));
        tmp = tmp->next;
    }
    ft_list_sort(&new_lst_env, ft_strcmp);
    add_quote(&new_lst_env);
    return (new_lst_env);
}

int    ft_export(char **cmds)
{
    t_list *env_to_print;
    size_t len_cmds;

    if (!cmds || !*cmds)
        return (ERROR);
    len_cmds = ft_strslen(cmds);
    add_env(cmds);
    if (len_cmds == 1)
    {
        env_to_print = env_export(&singleton()->env);
        ft_lstprint(env_to_print, '\n');
        ft_lstclear(&env_to_print, free);
    }
    return (SUCCESS);
}