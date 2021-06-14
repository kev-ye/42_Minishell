/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:45 by kaye              #+#    #+#             */
/*   Updated: 2021/06/14 18:15:31 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_free_ft_exit(void)
{
	ft_lstclear(&singleton()->lst, free);
	if (singleton()->cwd)
		ft_memdel((void **)(&singleton()->cwd));
	if (singleton()->env)
		ft_lstclear(&singleton()->env, free);
	if (singleton()->hist.path)
		ft_memdel((void **)(&singleton()->hist.path));
	close(singleton()->hist.fd);
	free(singleton());
}

static int check_neg_strdigit(char *strdigit)
{
	if (!ft_strncmp(strdigit, "-", 1) && ft_strisdigit(strdigit + 1) == 1)
		return (1);
	return (0);
}

int	ft_exit(char **cmds)
{
	size_t len;
	uint8_t ret;
	int	ret_no_num;

	ret = 0;
	ret_no_num = 0;
	len = ft_strslen(cmds);
	ft_putstr_fd("exit\n", STDIN_FILENO);
	if (len > 2 && cmds[1] && (ft_strisdigit(cmds[1]) == 1 || check_neg_strdigit(cmds[1])))
		ft_dprintf(STDERR_FILENO, PROG_NAME": %s: too many arguments\n", cmds[0]);
	else if (len >= 2 && cmds[1] && ft_strisdigit(cmds[1]) == 0 && !check_neg_strdigit(cmds[1]))
	{
		ft_dprintf(STDERR_FILENO, PROG_NAME": %s: %s: numeric argument required\n", cmds[0], cmds[1]);
		ret_no_num = 1;
	}
	if (ft_strisdigit(cmds[1]) || check_neg_strdigit(cmds[1]))
		ret = (uint8_t)ft_atoi(cmds[1]);
	if (ret_no_num == 1)
		ret = NO_NUM_ARG;
	ft_free_ft_exit();
	exit(ret);
}

void	ft_exit_for_prompt(void)
{
	if (singleton()->isatty_stdin)
		ft_putstr_fd("exit\n", STDIN_FILENO);
	ft_free_exit(SUCCESS);
}
