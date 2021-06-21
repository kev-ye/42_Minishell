/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:11:08 by kaye              #+#    #+#             */
/*   Updated: 2021/06/21 18:32:44 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_old_pwd(void)
{
	char	*path;

	path = getcwd(NULL, 0);
	if (!path)
		return (NULL);
	return (path);
}

int	check_if_oldpwd_exist(t_list *env)
{
	while (env)
	{
		if (env && !ft_strncmp((char *)env->content, "OLDPWD=", 7))
			return (1);
		env = env->next;
	}
	return (0);
}

void	replace_pwd(char *old_pwd, char *new_pwd, t_list *env)
{
	int	oldpwd_check;

	oldpwd_check = check_if_oldpwd_exist(env);
	if (!oldpwd_check)
		ft_lstadd_back(&env, ft_lstnew((void *)old_pwd));
	while (env)
	{
		if (env && !ft_strncmp((char *)env->content, "PWD=", 4))
		{
			free(env->content);
			env->content = new_pwd;
		}
		if (env && !ft_strncmp((char *)env->content, "OLDPWD=", 7)
			&& oldpwd_check)
		{
			free(env->content);
			env->content = old_pwd;
		}
		env = env->next;
	}
}

void	update_pwd_env(char *old_pwd)
{
	char	*new_pwd;
	char	*old_replacement;
	char	*tmp;

	tmp = getcwd(NULL, 0);
	if (!tmp)
		return ;
	ft_asprintf(&new_pwd, "PWD=%s", tmp);
	ft_asprintf(&old_replacement, "OLDPWD=%s", old_pwd);
	replace_pwd(old_replacement, new_pwd, singleton()->env);
	ft_memdel((void **)&tmp);
}

int	ft_cd(t_cmd *cmds)
{
	char	*old_pwd;

	if (!cmds || !cmds->args || !*cmds->args)
		return (ERROR);
	if (*cmds->args && !*(cmds->args + 1))
	{
		if (chdir(getenv("HOME")) == -1)
		{
			ft_dprintf(STDERR_FILENO, "%s: cd: HOME not set\n", PROG_NAME);
			return (ERROR);
		}
	}
	if (*cmds->args && *(cmds->args + 1) && chdir(cmds->args[1]) == -1)
	{
		ft_dprintf(STDERR_FILENO, "%s: cd: %s: %s\n",
			PROG_NAME, cmds->args[1], strerror(errno));
		return (ERROR);
	}
	else
	{
		old_pwd = get_old_pwd();
		update_pwd_env(old_pwd);
		ft_memdel((void **)&old_pwd);
	}
	return (SUCCESS);
}
