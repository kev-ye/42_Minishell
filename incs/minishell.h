/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/05/26 14:21:03 by besellem         ###   ########.fr       */
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

# define FOUND 0
# define NOT_FOUND -1
// # define PATH_MAX_LEN 256

# define PARSER_LIMITS_CHARS ";|<> "

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PRINT_ERR(s) ft_printf("\e[1;31m" __FILE__ ":" TOSTRING(__LINE__) \
						":\e[0m " s "\n");
// #define PRINT_ERR(s) (void)s;


/*
** -- DATA STRUCTURES --
*/

/*
** FLG_EO			// end of line
** FLG_EO_CMD		// `;'
** FLG_PIPE			// `|'
** FLG_OUTPUT		// `>'
** FLG_APPEND		// `>>'
** FLG_INPUT		// `<'
*/
enum	e_flags
{
	FLG_EOL =  0U,
	FLG_EO_CMD = (1U << 0),
	FLG_PIPE = (1U << 1),
	FLG_OUTPUT = (1U << 2),
	FLG_APPEND = (1U << 3),
	FLG_INPUT = (1U << 4)
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
	t_list	*env;				// env list
	// char	**cmds;
	t_list	*lst;				// main list containing all parsed commands
	int		last_return_value;	// last return value ($?)
	char	*cwd;				// pwd (mainly for `prompt' function)
}	t_minishl;

typedef struct s_builtin
{
	char *cmd;
	int (*f1)(char **cmds);
	int (*f2)(void);
}	t_builtin;

struct s_redirections
{
	char	*redir;
	int		len;
	uint8_t	flag;
};

typedef struct s_quotes
{
	int	s_quote;	// got a single quote
	int	d_quote;	// got a double quote
	int	first;		// got a quote
	int	did_change;	// did the entry quote has been found again ? (so it's the end of a string)
}	t_quotes;

/*
** -- PROTOTYPES --
*/

/*
** Utils
*/
void		ft_printstrs(int fd, char **strs);
void		ft_lstprint(t_list *lst, char sep);
void		ft_lstprint_cmd(t_list *lst);
char		**ft_lst2strs(t_list **lst);
void		ft_list_sort(t_list **begin_list, int (*cmp)());
int			ft_find_in_strs(char *s, const char **strs);
char		*ft_strclean(char *s, const char *charset);
char		*ft_strnclean(char *s, const char *charset, size_t end);
void		ft_free_exit(void) __attribute__((noreturn));
void		*ft_malloc_error(char *file, int line);

/*
** Parser
*/
t_minishl	*singleton(void);
char		*search_executable(char *command);
char		*search_builtin_executable(char *command);
int			quotes2close(unsigned char c, t_quotes *quotes, int status);
void		ft_parse(char *s);
void		ft_exec_each_cmd(void);
t_list		*search_env(char *tofind, t_list **env);

/*
** Builtin
*/
int			ft_echo(char **cmds);
int			ft_cd(char **cmds);
int			ft_pwd(void);
int			ft_env(char **cmds);
int			ft_export(char **cmds);
int			ft_unset(char **cmds);
int			ft_exit(void) __attribute__((noreturn));
int			ft_clear(void);

/*
** Signals
*/
void		ft_interrupt(int code);

void		print_prompt(void);

#endif
