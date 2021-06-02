/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:24:23 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 23:14:59 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TERMCAPS_H
# define FT_TERMCAPS_H

# define _TERMCAPS_ARROW_LEN 3

// Key codes
# define K_DELETE 0x7F
# define K_CTRL_D 0x4
# define K_CTRL_L 0xC
# define K_CTRL_U 0x15
# define K_UP "\x1b\x5b\x41"
# define K_DOWN "\x1b\x5b\x42"
# define K_RIGHT "\x1b\x5b\x43"
# define K_LEFT "\x1b\x5b\x44"

void	ft_termcap_history(char **ptr, char *termcap);
void	ft_termcap_edition(char **ptr, char *termcap);
void	ft_termcap_delete_char(char **ptr);
void	ft_termcap_clear_line(char **ptr);
void	ft_termcap_clear_screen(char **ptr);
void	ft_termcap_esc(char **ptr);

#endif
