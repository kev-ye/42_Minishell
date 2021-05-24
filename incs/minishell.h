/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 14:24:30 by besellem         ###   ########.fr       */
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

enum	e_flags
{
	FLG_EOL =  0U,				// end of line
	FLG_EO_CMD = (1U << 0),		// `;'
	FLG_PIPE = (1U << 1),		// `|'
	FLG_OUTPUT = (1U << 2),		// `>'
	FLG_APPEND = (1U << 3),		// `>>'
	FLG_INPUT = (1U << 4)		// `<'
};

typedef struct s_cmd
{
	char		**args;
	int			args_len;
	uint8_t		status_flag;	// used with e_flags's flags
	int			fd;				// for bonus (aggregation fd)
}	t_cmd;

typedef struct s_minishl
{
	char	**env;					// env list
	t_list	*lst;					// main list containing all parsed commands
	int		last_return_value;		// last return value ($?)
	char	*cwd;					// pwd (mainly for `prompt' function)
}	t_minishl;

typedef struct s_builtin
{
	char *cmd;
	int (*f1)(char **cmds);
	int (*f2)(void);
}	t_builtin;

typedef struct s_quotes
{
	int	sgl;	// single quotes
	int	dbl;	// double quotes
}	t_quotes;

struct s_redirections
{
	char	*redir;
	uint8_t	flag;
};

/*
** -- PROTOTYPES --
*/

/*
** Utils
*/
void		ft_printstrs(int fd, char **strs);
void		ft_lstprint(t_list *lst, char sep);
void		ft_lstprint_cmd(t_list *lst, char sep);
int			ft_find_in_strs(char *s, const char **strs);
char		*ft_strclean(char *s, const char *charset);
char		*ft_strnclean(char *s, const char *charset, size_t end);
void		ft_free_exit(void);

/*
** Parser
*/
t_minishl	*singleton(void);
char		*search_executable(char *command);
char		*search_builtin_executable(char *command);
int			quotes2close(char c, int reinit);
void		ft_parse(char *s);
void		ft_exec_each_cmd(void);

/*
** Builtin
*/
int		ft_echo(char **cmds);
int    	ft_cd(char **cmds);
int		ft_pwd(void);
int   	ft_env(char **cmds);
int 	ft_unset(char **cmds);
int   	ft_exit(void);// __attribute__((noreturn));

#endif
