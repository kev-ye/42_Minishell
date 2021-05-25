/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clear.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 10:54:53 by besellem          #+#    #+#             */
/*   Updated: 2021/05/25 11:06:06 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_clear(void)
{
	ft_putstr_fd(CLR_SCREEN, STDOUT_FILENO);
	return (SUCCESS);
}
