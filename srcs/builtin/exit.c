/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:45 by kaye              #+#    #+#             */
/*   Updated: 2021/06/09 13:44:26 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_exit(void)
{
	if (singleton()->isatty_stdin)
		ft_putstr_fd("exit\n", STDIN_FILENO);
	ft_free_exit();
	exit(EXIT_SUCCESS);
}
