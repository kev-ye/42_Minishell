/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/06/09 18:38:58 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

////////////////////////////////////////////////////////////////////////////////
// -- INCLUDES --
////////////////////////////////////////////////////////////////////////////////
// System's
# include <stdio.h>
# include <stdint.h>
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

# include <readline/readline.h>
# include <readline/history.h>

// Custom
# include "libft.h"
# include "ft_termcaps.h"

////////////////////////////////////////////////////////////////////////////////
// -- DEFINES --
////////////////////////////////////////////////////////////////////////////////
// Program name
# define PROG_NAME "minishell"

// Prompt
# define PROMPT "\e[1;36m\e[1m%s \e[1;31m$ \e[0m"
# define PROMPT_CPADDING 3

// History file. May be renamed at compile time
# ifndef HISTORY_FILENAME
#  define HISTORY_FILENAME ".minishell_history"
# endif

/*
** Used to add an entry to history only if it's different from the last one.
** For this purpose, a simple strcmp is called between the last and the new
** entry.
** This is the behavour of zsh.
** Bash however adds it whatever the last one was. To replicate this behavour,
** just set -D ZSH_HISTORY_HANDLING=0 at compile time.
*/
# ifndef ZSH_HISTORY_HANDLING
#  define ZSH_HISTORY_HANDLING 1	// to remove - unused
# endif

/*
** Some simple & useful macros
*/
# define SUCCESS 0
# define ERROR 1

# define FOUND 0
# define NOT_FOUND (-1)

# ifndef TRUE
#  define TRUE 1
# endif
# if defined(TRUE) && 1 != TRUE		/* May be set already */
#  undef TRUE
#  define TRUE 1
# endif

# ifndef FALSE
#  define FALSE 0
# endif
# if defined(FALSE) && 0 != FALSE	/* May be set already */
#  undef FALSE
#  define FALSE 0
# endif

// Exec
# define SYNTAXERR "syntax error near unexpected token"
# define SIMPLE 0
# define ONLY_PIPE 1
# define ONLY_REDIR 2
# define MIX 4

// Bultin
# define NO_NUM_ARG 255

// Exec ret for $?
# define PID_FAILURE 1
# define EXEC_FAILURE 1
# define LRV_SYNTAX_ERROR 258
# define LRV_CMD_NOT_FOUND 127
# define LRV_GENERAL_ERROR 1
# define LRV_KILL_SIG 128

// SET BONUS TO 0 BY DEFAULT
# ifndef BONUS
#  define BONUS 0
# endif

/*
** LRV -> LAST RETURN VALUE
** Some return codes :
*/
// Command not found
# define LRV_NOT_FOUND 127

// # define PATH_MAX_LEN 256

// DEBUGGING PURPOSE - TO REMOVE
# define STRINGIFY(x) #x
# define TOSTRING(x) STRINGIFY(x)
# define PRINT_ERR(s) ft_printf("\e[1;31m" __FILE__ ":" TOSTRING(__LINE__) \
						":\e[0m " s "\n");
// END DEBUGGING PURPOSE - TO REMOVE

// Charsets used in the parsing
// # define SPEC_CHARS " \\$'\""
# define SPACES " \t"
# define QUOTES "\"'"

/*
** Bonus macro
** Set to 0 by default
*/
# ifndef BONUS
#  define BONUS 1


//##################################
// REDEFINE BONUS TO 0 WHEN FINISHED
//##################################

// #  define BONUS 0

# endif

////////////////////////////////////////////////////////////////////////////////
// -- DATA STRUCTURES --
////////////////////////////////////////////////////////////////////////////////
/*
** Used for the parsing to cut the command when the following chars are found:
**
** FLG_EO_CMD:			`;'
** FLG_PIPE:			`|'
** FLG_OUTPUT:			`>'
** FLG_APPEND:			`>>'
** FLG_INPUT:			`<'
** FLG_DINPUT:			`<<'
** FLG_EOL:				`\0' (end of line)
*/
enum	e_flags
{
	FLG_EO_CMD = (1U << 0),
	FLG_PIPE = (1U << 1),
	FLG_OUTPUT = (1U << 2),
	FLG_APPEND = (1U << 3),
	FLG_INPUT = (1U << 4),
	FLG_DINPUT = (1U << 5),
	FLG_EOL = (1U << 6)
};

// Only used whithin a lookup table for the parsing
struct s_redirections
{
	char	*redir;
	int		len;
	uint8_t	flag;
};

// Only used whithin a lookup table to execute a command based on key codes
struct s_termcaps
{
	char	*termcap;
	void	(*f)();
};

/*
** Used to define the status of the quotes in the parsing:
**
** s_quote:				got a single quote
** d_quote:				got a double quote
** first:				got a quote
** did_change:			did the entry quote has been found again?
**						(so it's the end of a string)
*/
typedef struct s_quotes
{
	int		s_quote;
	int		d_quote;
	int		first;
	int		did_change;
}	t_quotes;

/*
** Keeps some important infos about the history:
**
** fd:					fd of the HISTORY_FILENAME created & opened
** current:				current history entry
** size:				size of all the entries
** path:				path of the file
** history:				list containing all entries
*/
typedef struct s_history
{
	int		fd;
	char	*path;
}	t_history;

/*
** Keeps track of the cursor's position:
**
** len:					total len of the current cmd (minus the prompt's len)
** line:				line number
** current_index:		current cursor position on the string
** current_line:		current line number (in case of a small terminal)
*/
typedef struct s_edition
{
	size_t	len;
	size_t	line;
	size_t	current_index;
	size_t	current_line;
}	t_edition;

/*
** Actual commands parsed, almost ready to be executed.
**
** args:				arguments of a command (may be NULL)
** args_len:			len of the arguments of the command
** status_flag:			used with the enum's (e_flags) flags
** fd:					for bonus (aggregation fd)
*/
typedef struct s_cmd
{
	char		**args;
	int			args_len;
	uint8_t		status_flag;
	int			fd;
}	t_cmd;

/*
** Used for builtin's execution
**
** cmd:					the command (`cd' for example)
** f1:					a function call with arguments (like `ft_export')
** f2:					a function call without arguments (like `ft_exit')
*/
typedef struct s_builtin
{
	char	*cmd;
	int		(*f1)(char **cmds);
	int		(*f2)(void);
}	t_builtin;

/*
** Used to handle arguments in the program
**
** opt_c:				is the -c option found (bool)
** fd:					if there's a second arg and it's a file, execute the
						script (the commands actually - nor loop or conditions
						are implemented). Otherwise, set the fd to STDIN_FILENO.
*/
struct s_options
{
	uint8_t	opt_c : 2;
	int		fd;
};

/*
** Main stucture. Called with a singleton.
**
** isatty_stdin:		checks if the STDIN_FILENO fd is a terminal
** last_return_value:	last return value of a command ($?)
** cwd:					pwd (mainly used in the `prompt' function)
** cwd_basename:		only the basename of the current directory
** env:					list of the environment variables
** lst:					main list containing all parsed commands
**						(t_cmd *)(lst->content)
** edit:				struct used for edition
** hist:				struct used for history
** tattr:				terminal's attributes (like the non-canonical mode)
*/
typedef struct s_minishl
{
	int					isatty_stdin;
	int					last_return_value;
	char				*cwd;				// one of those must be thrown
	char				*cwd_basename;		// one of those must be thrown
	char				*prompt;			// one of those must be thrown
	t_list				*env;
	t_list				*lst;
	t_edition			edit;
	t_history			hist;
	struct s_options	option;
	struct termios		tattr;
}	t_minishl;

////////////////////////////////////////////////////////////////////////////////
// -- PROTOTYPES --
////////////////////////////////////////////////////////////////////////////////
// Utils
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

// Parser
t_minishl	*singleton(void);
int			ft_gnl(int fd, char **line);
char		*search_executable(char *command);
int			quotes2close(unsigned char c, t_quotes *quotes, int status);
void		ft_parse(char *s);
t_list		*search_env(char *tofind, t_list **env);

/*
** Execution
*/
void		ft_exec_each_cmd(t_list *lst);
char		*search_executable(char *command);
void		ft_pre_exec_cmd(void *ptr);
void 		cmd_with_pipe(t_list *lst_cmd);
void		cmd_with_redir(void *cmd, t_list *lst_cmd);
int 		part_cmd_check(t_list *lst_cmd);
int 		syntax_parser(t_list *lst_cmd);
int 		builtin_exec(char **cmds);
void		sys_exec(void *ptr);
/*
** Builtin
*/
int			ft_echo(char **cmds);
int			ft_cd(char **cmds);
int			ft_pwd(void);
int			ft_env(char **cmds);
int			ft_export(char **cmds);
int			ft_unset(char **cmds);
void		ft_exit_for_prompt(void);
int			ft_exit(char **cmds) __attribute__((noreturn));
int			ft_clear(void);

void		print_inline(char **ptr, char *buffer);

// Signals
void		ft_interrupt(int code);

void		print_prompt(void);

#endif
