/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 17:55:25 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/*
** -- INCLUDES --
*/
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <dirent.h>
# include <termios.h>
# include <curses.h>
# include <term.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/errno.h>
# include <sys/ioctl.h>
// # include <sys/param.h>

# include "libft.h"

/*
** -- DEFINES --
*/
# define PROG_NAME "minishell"

// # define SUCCESS 0
// # define ERROR 1

/*
** -- DATA STRUCTURES --
*/

enum	e_state
{
	SUCCESS,
	ERROR
};

enum	e_search
{
	FOUND,
	NOT_FOUND = -1
};

typedef struct s_dlist
{
	void			*data;
	struct s_dlist	*prev;
	struct s_dlist	*next;
} t_dlist;

typedef struct s_fptr
{
	char	*data;
	int		(*f)();
} t_fptr;

typedef struct s_cmd
{
	char	**cmd;
	int		pipe;
	int		redirect;
} t_cmd;

typedef struct s_minishl
{
	char	**env;
	char	**cmds;
	t_list	*lst;
} t_minishl;

/*
** -- PROTOTYPES --
*/
char	*search_executable(char *s);

#endif
