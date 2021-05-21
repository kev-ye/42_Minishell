/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/05/21 16:52:08 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/*
** -- INCLUDES --
*/
# include <stdio.h>
# include <stdint.h>
// # include <limits.h>
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

# define CUSTOM_T_LIST
# include "libft.h"

/*
** -- DEFINES --
*/
# define PROG_NAME "minishell"
# define PROMPT "\e[1;36m\e[1m%s \e[1;31m$ \e[0m"

# define SUCCESS 0
# define ERROR 1
// # define PATH_MAX_LEN 256

/*
** -- DATA STRUCTURES --
*/
enum	e_search
{
	FOUND,
	NOT_FOUND = -1
};

enum	e_redirect_flags
{
	PIPE = (1UL << 0),
	REDIRECTION = (1UL << 1)
};

typedef struct s_dlist
{
	void			*data;
	struct s_dlist	*prev;
	struct s_dlist	*next;
}	t_dlist;

typedef struct s_fptr
{
	char	*data;
	int		(*f)();
}	t_fptr;

typedef struct s_cmd
{
	char	**av;
	int		ac;
	int		pipe;
	int		redirect;
}	t_cmd;

typedef struct s_minishl
{
	char	**env;
	char	**cmds;
	t_list	*lst;
	int		last_return_value;
}	t_minishl;

/*
** -- PROTOTYPES --
*/

/*
** Utils
*/
void		ft_printstrs(int fd, char **strs);
void		ft_lstprint(t_list *lst, char sep);
int			ft_find_in_strs(char *s, const char **strs);
void		ft_free_exit(void);

/*
** Parser
*/
t_minishl	*singleton(void);
char		*search_executable(char *command);
int			got_quotes(int add_single, int add_double, int reinit);
void		ft_parse(char *s);
void		ft_exec_each_cmd(void);

/*
** Builtin
*/
void   ft_echo(t_cmd *cmd);
int    ft_cd(const char *path);
int    ft_pwd(void);
int    ft_env(t_cmd *cmd);
void   ft_exit(void);

#endif
