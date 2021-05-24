/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 15:16:48 by besellem         ###   ########.fr       */
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
# define BUILTIN 7

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
	char		**args;
	int			args_len;
	uint8_t		status_flag;	// used with e_flags's flags
	int			fd;				// for bonus (aggregation fd)
}	t_cmd;

typedef struct s_minishl
{
	t_list *env;
	char	**cmds;
	t_list	*lst;
	int		last_return_value;
	t_cmd 	*cmd;
}	t_minishl;

typedef struct s_builtin
{
	char *cmd;
	int (*f1)(char **cmds);
	int (*f2)(void);
}	t_builtin;

/*
** -- PROTOTYPES --
*/

/*
** Utils
*/
void		ft_printstrs(int fd, char **strs);
void		ft_lstprint(t_list *lst, char sep);
int			ft_find_in_strs(char *s, const char **strs);
char		*ft_strclean(char *s, const char *charset);
char		*ft_strnclean(char *s, const char *charset, size_t end);
void		ft_free_exit(void);
void 		ft_list_sort(t_list **begin_list, int (*cmp)());
t_list		*ft_lstnew_env(void *content);
t_list  	*search_env(char *tofind, t_list **env);

/*
** Parser
*/
t_minishl	*singleton(void);
char		*search_executable(char *command);
int			got_quotes(int add_single, int add_double, int reinit);
void		ft_parse(char *s);
void		ft_exec_each_cmd(void);
////////////////////// kaye
char	*search_builtin_executable(char *command);
////////////////////////

/*
** Builtin
*/
int		ft_echo(char **cmds);
int    	ft_cd(char **cmds);
int		ft_pwd(void);
int   	ft_env(char **cmds);
int    	ft_export(char **cmds);
int 	ft_unset(char **cmds);
int   	ft_exit(void);

#endif
