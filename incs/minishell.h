/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:07:35 by besellem          #+#    #+#             */
/*   Updated: 2021/06/22 12:25:47 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

////////////////////////////////////////////////////////////////////////////////
// -- INCLUDES --
////////////////////////////////////////////////////////////////////////////////

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

# include "libft.h"
# include "minishell_defs.h"
# include "ft_termcaps.h"

////////////////////////////////////////////////////////////////////////////////
// -- DATA STRUCTURES --
////////////////////////////////////////////////////////////////////////////////
/*
** Used for the parsing to cut the commands when the following chars are found:
**
** FLG_EO_CMD:			`;'
** FLG_PIPE:			`|'
** FLG_OUTPUT:			`>'
** FLG_APPEND:			`>>'
** FLG_INPUT:			`<'
** FLG_DINPUT:			`<<'
** FLG_EOL:				`\0' (end of line)
** FLG_LRV:				`$?' (code that needs an expansion has been found)
*/
enum	e_flags
{
	FLG_EO_CMD = (1U << 0),
	FLG_PIPE = (1U << 1),
	FLG_OUTPUT = (1U << 2),
	FLG_APPEND = (1U << 3),
	FLG_INPUT = (1U << 4),
	FLG_DINPUT = (1U << 5),
	FLG_EOL = (1U << 6),
	FLG_LRV = (1U << 7)
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
*/
typedef struct s_cmd
{
	char	**args;
	int		args_len;
	uint8_t	status_flag;
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
	int		(*f1)(t_cmd *cmds);
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
	char				*cwd;
	char				*cwd_basename;
	char				*prompt;
	t_list				*env;
	t_list				*lst;
	t_edition			edit;
	t_history			hist;
	struct s_options	option;
	struct termios		tattr;
	int					rl_lvl;
}	t_minishl;

////////////////////////////////////////////////////////////////////////////////
// -- PROTOTYPES --
////////////////////////////////////////////////////////////////////////////////
/* Utils */
void		*ft_error(char *message, char *file, int line);
int			ft_is_openable(char *path, int flag);
char		*ft_getenv(const char *name);
char		*ft_strnclean(char *s, const char *charset, size_t end);
void		ft_lstprint(t_list *lst, char sep);
void		ft_list_sort(t_list **begin_list, int (*cmp)());
char		**ft_lst2strs(t_list **lst);
void	ft_lstprint_cmd(t_list *); // TO REMOVE DEBUG PURPOSE

/* Memory Management */
void		__ft_free_cmds__(void);
void		ft_free_exit(int code);

/* General */
t_minishl	*singleton(void);
void		print_prompt(void);
char		*search_executable(char *command);
t_list		*search_env(char *tofind, t_list **env);

/* Signals */
void		ft_interrupt(int code);

/* Parser */
int			quotes2close(unsigned char c, t_quotes *quotes, int status);
void		ft_parse(char *s);

/* Execution */
// General
int			ft_exec_each_cmd(t_list *lst);
char		*search_executable(char *command);
void		ft_pre_exec_cmd(void *ptr);
int			part_cmd_check(t_list *lst_cmd);
int			syntax_parser(t_list *lst_cmd);
int			builtin_exec(t_cmd *cmds);
int			sys_exec(void *ptr);
int			check_if_path_exist(t_list *env);

// Pipe
void		*first_cmd_with_pipe(void *cmd, int *fd);
void		interm_cmd_with_pipe(void *cmd, int *fd, int fd_index);
void		last_cmd_with_pipe(void *cmd, int *fd, int fd_index);
void		cmd_with_multi_pipe(t_list *lst_cmd, int *fd);
int			count_pipe(t_list *lst_cmd);
void		cmd_with_pipe(t_list *lst_cmd);

// Redirections
void		*get_complete_cmd(void *cmd, t_list *lst_cmd);
void		redir_parser(int fd_input, int fd_output, t_list *lst_cmd);
void		cmd_with_redir(void *cmd, t_list *lst_cmd);
void		create_fd(t_list *cmd);
void		create_fd_input(t_list *cmd);
int			check_for_next(t_list *lst_cmd);
char		*get_tmp_fd(int i);
void		redir_parser2(t_list *cmd, int *fd_input, int *fd_output);

// Redirections Mix
void		cmd_with_mix(t_list *lst_cmd);
void		cmd_with_pipe_mix(t_list *lst_cmd);

int			simple_cmd(void *cmd);
void		exec_all_in_one(t_list *lst_cmd);

// debug to delete
void	show_content(t_list *, char *);
void	show_fd(int, char *);

/* Flag */
int			flag_check(t_list *lst_cmd);
int			is_redir(t_list *lst_cmd);
int			is_sep_or_end(t_list *lst_cmd);

/* Builtins */
int			ft_echo(t_cmd *cmds);
int			ft_cd(t_cmd *cmds);
int			ft_pwd(void);
int			ft_env(t_cmd *cmds);
int			ft_export(t_cmd *cmds);
void		add_quote(t_list **lst_env);
char		*head_env(char *env);
int			check_space(char *s);
int			ft_unset(t_cmd *cmds);
void		ft_exit_for_prompt(void);
int			ft_exit(t_cmd *cmds);
int			ft_clear(void);

#endif
