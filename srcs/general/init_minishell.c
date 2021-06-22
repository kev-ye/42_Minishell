/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_minishell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/22 12:17:59 by besellem          #+#    #+#             */
/*   Updated: 2021/06/22 12:19:06 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*ft_get_shlvl(char *shlvl)
{
	char	*ret;
	int		level;

	if (!shlvl)
		return (ft_strdup("SHLVL=1"));
	level = ft_atoi(shlvl);
	if (ft_strlen(shlvl) > (size_t)ft_nblen(INT32_MAX) || (level + 1) < 0)
		level = 0;
	else if (999 == level)
		return (ft_strdup("SHLVL="));
	else if (level >= 1000)
	{
		ft_dprintf(STDERR_FILENO,
			PROG_NAME ": warning: shell level (%ld) too high, resetting to 1\n",
			level + 1);
		level = 1;
	}
	else
		++level;
	ft_asprintf(&ret, "SHLVL=%d", level);
	if (!ret && NULL == ft_memdel((void **)&shlvl))
		return (ft_error(ERR_MALLOC, __FILE__, __LINE__));
	return (ret);
}

static int	ft_init_minishell(char **env)
{
	const char	*args[] = {"export", NULL, NULL};
	const t_cmd	cmd = {.args = (char **)args, .args_len = 0, .status_flag = 0};
	char		*ret;
	char		*shlvl;

	if (NULL == singleton())
		return ((int)ft_error(ERR_MALLOC, __FILE__, __LINE__));
	singleton()->env = get_env(env);
	shlvl = ft_getenv("SHLVL");
	ret = ft_get_shlvl(shlvl);
	args[1] = ret;
	ft_export((t_cmd *)&cmd);
	ft_memdel((void **)&ret);
	ft_memdel((void **)&shlvl);
	singleton()->isatty_stdin = isatty(STDIN_FILENO);
	signal(SIGQUIT, ft_interrupt);
	signal(SIGINT, ft_interrupt);
	return (1);
}
