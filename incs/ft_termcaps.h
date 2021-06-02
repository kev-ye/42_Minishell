/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:24:23 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 14:37:02 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TERMCAPS_H
# define FT_TERMCAPS_H

#define _TERMCAPS_ARROW_LEN 3

// Key codes
#define K_UP "\x1b\x5b\x41"
#define K_DOWN "\x1b\x5b\x42"
#define K_RIGHT "\x1b\x5b\x43"
#define K_LEFT "\x1b\x5b\x44"

void	ft_termcap_history(char *termcap);
void	ft_termcap_edition(char *termcap);

#endif
