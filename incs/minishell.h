/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/06/03 17:15:20 by besellem         ###   ########.fr       */
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

# include "libft.h"
# include "ft_termcaps.h"

/*
** -- DEFINES --
*/
# define PROG_NAME "minishell"

# define PROMPT_CPADDING 3
# define PROMPT "\e[1;36m\e[1m%s \e[1;31m$ \e[0m"

# ifndef HISTORY_FILENAME
#  define HISTORY_FILENAME ".minishell_history"
# endif

# define SUCCESS 0
# define ERROR 1

# define FOUND 0
# define NOT_FOUND (-1)

// Define TRUE macro to 1
# ifndef TRUE
#  define TRUE 1
# endif
# if defined(TRUE) && 1 != TRUE
#  undef TRUE
#  define TRUE 1
# endif

// Define FALSE macro to 0
# ifndef FALSE
#  define FALSE 0
# endif
# if defined(FALSE) && 0 != FALSE
#  undef FALSE
#  define FALSE 0
# endif

// SET BONUS TO 0 BY DEFAULT
# ifndef BONUS
#  define BONUS 0
# endif

# ifndef ZSH_HISTORY_HANDLING
#  define ZSH_HISTORY_HANDLING 1
# endif
// # if defined(ZSH_HISTORY_HANDLING) && (ZSH_HISTORY_HANDLING != 1)
// #  undef ZSH_HISTORY_HANDLING
// #  define ZSH_HISTORY_HANDLING 1
// # endif

// # define PATH_MAX_LEN 256

# define PARSER_LIMITS_CHARS ";|<> "

// DEBUGGING PURPOSE - TO REMOVE
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PRINT_ERR(s) ft_printf("\e[1;31m" __FILE__ ":" TOSTRING(__LINE__) \
						":\e[0m " s "\n");
// END DEBUGGING PURPOSE - TO REMOVE

/*
** -- DATA STRUCTURES --
*/

/*
** Used for builtin's execution
*/
typedef struct s_builtin
{
	char	*cmd;
	int		(*f1)(char **cmds);
	int		(*f2)(void);
}	t_builtin;

struct s_redirections
{
	char	*redir;
	int		len;
	uint8_t	flag;
};

struct s_termcaps
{
	char	*termcap;
	void	(*f)();
};

/*
** Used for the parsing
**
** s_quote:		got a single quote
** d_quote:		got a double quote
** first:		got a quote
** did_change:	did the entry quote has been found again?
				(so it's the end of a string)
*/
typedef struct s_quotes
{
	int	s_quote;
	int	d_quote;
	int	first;
	int	did_change;
}	t_quotes;

/*
** Used for the parsing to cut the commands when the following chars are found
**
** FLG_EO			// end of line
** FLG_EO_CMD		// `;'
** FLG_PIPE			// `|'
** FLG_OUTPUT		// `>'
** FLG_APPEND		// `>>'
** FLG_INPUT		// `<'
*/
enum	e_flags
{
	FLG_EO_CMD = (1U << 0),
	FLG_PIPE = (1U << 1),
	FLG_OUTPUT = (1U << 2),
	FLG_APPEND = (1U << 3),
	FLG_INPUT = (1U << 4),
	FLG_EOL = (1U << 5)
};

/*
** Actual commands parsed, almost ready to be executed
**
** args:			arguments of a command
** args_len:		len of all the arguments of the command
** status_flag:		used with e_flags's flags
** fd:				for bonus (aggregation fd)
*/
typedef struct s_cmd
{
	char		**args;
	int			args_len;
	uint8_t		status_flag;
	int			fd;
}	t_cmd;

typedef struct s_history
{
	int		fd;
	size_t	current;
	size_t	size;
	char	*path;
	t_list	*history;
}	t_history;

typedef struct s_edition
{
	size_t	len;
	size_t	current_index;
}	t_edition;

/*
** Main stucture. Called with a singleton
**
** env:						env list
** lst:						main list containing all parsed commands
** last_return_value:		last return value ($?)
** cwd:						pwd (mainly for `prompt' function)
*/
typedef struct s_minishl
{
	int				isatty_stdin;
	int				last_return_value;
	char			*cwd;
	char			*cwd_basename;
	t_list			*env;
	t_list			*lst;
	t_edition		edit;
	t_history		hist;
	struct termios	tattr;
}	t_minishl;

/*
** -- PROTOTYPES --
*/

/*
** Utils
*/
int			ft_sputchar(int c);
int			ft_is_openable(char *path, int flag);
void		ft_printstrs(int fd, char **strs);
void		ft_lstprint(t_list *lst, char sep);
t_list		*ft_lstindex(t_list *lst, size_t index);
void		ft_lstprint_cmd(t_list *lst);
char		**ft_lst2strs(t_list **lst);
void		ft_list_sort(t_list **begin_list, int (*cmp)());
int			ft_find_in_strs(char *s, const char **strs);
char		*ft_getenv(const char *name);
char		*ft_strclean(char *s, const char *charset);
char		*ft_strnclean(char *s, const char *charset, size_t end);
void		ft_free_exit(void) __attribute__((noreturn));
void		*ft_malloc_error(char *file, int line);

/*
** Parser
*/
t_minishl	*singleton(void);
int			ft_gnl_stdin(char **line);
char		*search_executable(char *command);
int			quotes2close(unsigned char c, t_quotes *quotes, int status);
void		ft_parse(char *s);
void		ft_exec_each_cmd(t_list *lst);
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
** History
*/
void		init_history(void);
void		add2history(char *cmd);

void		print_inline(char **ptr, char *buffer);

/*
** Signals
*/
void		ft_interrupt(int code);

void		print_prompt(void);

#endif
