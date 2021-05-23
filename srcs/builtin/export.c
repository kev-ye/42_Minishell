/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:47 by kaye              #+#    #+#             */
/*   Updated: 2021/05/23 20:07:26 by kaye             ###   ########.fr       */
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
        tmp = tmp->next;
    }
}

static int  search_env(char *tofind)
{
    char *new_env;
    size_t len_tofind;
    int i;

    len_tofind = 0;
    while (tofind[len_tofind] && tofind[len_tofind] != '=')
        ++len_tofind;
    i = 0;
    while (singleton()->env[i])
    {
        if (!ft_strncmp(singleton()->env[i], tofind, len_tofind))
        {
            new_env = ft_strdup(tofind);
            if (!new_env)
                return (0);
            ///
            /// last time here
            ///
            return (1);
        }
        ++i;
    }
    return (0);
}

// static void free_old_shell_env()
// {
//     int i;

//     i = 0;
//     if (singleton()->env[i])
//     {
//         free(singleton()->env[i]);
//         ++i;
//     }
//     free(singleton()->env);
// }

static void new_shell_env(char *new_env)
{
    char **new_shell_env;
    size_t shell_env_len;
    size_t env_len;
    int i;

    shell_env_len = ft_strslen(singleton()->env);
    new_shell_env = malloc(sizeof(char *) * (shell_env_len + 2));
    if (!new_shell_env)
        return ;
    i = -1;
    while (++i < (int)shell_env_len)
    {
        env_len = ft_strlen(singleton()->env[i]);
        new_shell_env[i] = malloc(sizeof(char) * env_len + 1);
        if (!new_shell_env[i])
            return ;
        // printf("wtf %s\n", singleton()->env[i]);
        ft_strcpy(new_shell_env[i], singleton()->env[i]);
    }
    new_shell_env[shell_env_len] = new_env;
    new_shell_env[shell_env_len + 1] = NULL;
    // free_old_shell_env();
    singleton()->env = new_shell_env;
}

static void add_env(char **cmds)
{
    char *new_env;
    int i;

    i = 1;
    while (cmds[i])
    {
        if (search_env(cmds[i]))
        {
            new_env = ft_strdup(cmds[i]);
            if (!new_env)
                return ;
            new_shell_env(new_env);
            // int a;
            // a = 0;
            // while (singleton()->env[a])
            // {
            //     printf("new : %s\n", singleton()->env[a]);
            //     a++;
            // }
            // exit(0);
            return ;
        }
        ++i;
    }
}

static t_list *new_env()
{
    t_list  *lst_env;
    char    *env;
    int i;

    i = 0;
    while(singleton()->env[i])
    {
        env = ft_strdup(singleton()->env[i]);
        ft_lstadd_back(&lst_env, ft_lstnew((void *)env));
        ++i;
    }
    ft_list_sort(&lst_env, ft_strcmp);
    add_quote(&lst_env);
    return (lst_env);
}

int    ft_export(char **cmds)
{
    t_list *lst_env;

    if (!cmds || !*cmds)
        return (ERROR);
    add_env(cmds);
    lst_env = new_env();
    while (lst_env)
    {
        ft_dprintf(STDIN_FILENO, "%s\n", (char *)lst_env->content);
        lst_env = lst_env->next;
    }
    ft_lstclear(&lst_env, free);
    return (SUCCESS);
}