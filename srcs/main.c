/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 17:58:05 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_printstrs(int fd, char **strs)
{
	size_t	i;

	i = 0;
	while (strs[i])
		ft_putendl_fd(strs[i++], fd);
	ft_putstr_fd("\n", fd);
}

t_minishl	*singleton(void)
{
	static t_minishl	*ptr = NULL;

	if (!ptr)
	{
		ptr = ft_calloc(1, sizeof(t_minishl));
		if (!ptr)
		{
			ptr = NULL;
			return (NULL);
		}
	}
	return (ptr);
}

int	builtin_cd(char *dir)
{
	char	*pwd;

	if (getenv("PWD"))
	{
		if (chdir(dir))
			perror(dir);
		pwd = getcwd(NULL, 0);
		setenv("PWD", pwd, 1);
		printf("pwd => [%s], [%s]\n", pwd, getenv("PWD"));
		free(pwd);
		return (0);
	}
	return (1);
}

int	builtin_exit(int code) __attribute__((noreturn));

int	builtin_exit(int code)
{
	exit(code);
}

int		ft_find_in_strs(char *s, const char **strs)
{
	size_t	i;

	if (!s || !strs || !*strs)
		return (-1);
	i = 0;
	while (strs[i])
	{
		if (ft_strcmp(s, strs[i]) == 0)
			return (i);
		++i;
	}
	return (-1);
}

#define COMPLETE_METACHARACTERS "|&;()<> \t"
#define METACHARACTERS "|;<> \t"
#define CTRL_OPERATORS "\n"

int	got_metacharacter(void);



void	parse(char *s)
{
	size_t	i;
	size_t	j;

	// ft_strsfree(ft_strslen(singleton()->lst), singleton()->lst);
	// ft_lstclear(singleton()->lst, NULL);
	(void)j;
	i = 0;
	while (s[i])
	{
		// ft_stroc(s, "")
		// ft_strchr(s + i, '"');
		// if (s[i] == '\'')
		// if (s[i] == ' ' || s[i] == '|' || s[i] == ';')
		// {
			
		// }
		++i;
	}


	// const t_fptr	cmds[] = {
	// 	"echo", "cd", "pwd", "export", "unset", "env", "exit", NULL
	// };

	// const t_fptr	cmds[] = {
	// 	{"cd", builtin_cd}, {"exit", builtin_exit}, {NULL, NULL}
	// };
	// size_t		i;

	// singleton()->cmds = ft_split(s, ' ');
	// i = 0;
	// while (singleton()->cmds[i])
	// {
	// 	int	ret = ft_find_in_strs(singleton()->cmds[i], cmds);
	// 	if (ret != -1)
	// 	{
	// 		cmds[ret].f();
	// 	}
	// 	++i;
	// }

	// if (*args_split && ft_strslen(singleton()->cmds) > 1 && ft_strcmp(*singleton()->cmds, "cd") == 0)
	// 	builtin_cd(singleton()->cmds[1]);
	// else if (*singleton()->cmds && ft_strcmp(*singleton()->cmds, "exit") == 0)
	// 	exit(0);

	// size_t i = 0;
	// while (singleton()->cmds[i])
	// {
	// 	ft_printf("[%s]", singleton()->cmds[i]);
	// 	if (singleton()->cmds[i + 1])
	// 		ft_printf(" ");
	// 	++i;
	// }
	
	// if (ft_strslen(singleton()->cmds) > 0)
	// 	ft_printf("\n");


	// singleton()->cmds = ft_strsplit(s, " ;");


	// ft_strsfree(ft_strslen(singleton()->cmds), singleton()->cmds);
}

void	ft_free_exit(void)
{
	free(singleton());
}

// must be used only on commands (forked) - not on the main program
void	ft_quit(int code)
{
	ft_dprintf(2, "Quit: %d\n", code);
	exit(code);
}

void	ft_interrupt(int code)
{
	// ft_dprintf(2, "Quit: %d\n", code);
	exit(code);
}

void	prompt(void)
{
	char	*ret;
	int		r;

	// signal(SIGQUIT, ft_quit);
	// signal(SIGINT, ft_interrupt);

	while (1)
	{
		char	*pwd = getcwd(NULL, 0);
		ft_dprintf(STDIN_FILENO, B_RED "\e[1m" "%s $" CLR_COLOR " ", ft_strrchr(pwd, '/') + 1);
		free(pwd);
		r = get_next_line(STDIN_FILENO, &ret);
		parse(ret);

		// EXEC HERE

		free(ret);
		if (r <= 0)
			break ;
	}
}

void	ft_lstprint(t_list *lst, char sep)
{
	t_list	*tmp;

	tmp = lst;
	while (tmp)
	{
		ft_printf("%s%c", tmp->content, sep);
		tmp = tmp->next;
	}
}

// void	ft_getenv(char **env)
// {
// 	t_list	*lst;
// 	size_t	i;

// 	i = ft_strslen(env);
// 	while (i-- > 0)
// 		ft_lstadd_front(&lst, ft_lstnew(env[i]));
// }

int	ft_exec_cmd(char *cmd)
{
	char		*commands[] = {cmd, NULL};
	const pid_t	id = fork();

	if (id == 0)
	{
		return (execve(cmd, commands, NULL));
	}
	else
	{
		wait(NULL);
	}
	return (ERROR);
}

int	main(__attribute__((unused))int ac,
		__attribute__((unused))const char **av,
		__attribute__((unused))char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	
	// ft_printstrs(STDOUT_FILENO, env);

	// pid_t id = fork();

	// if (id == 0)
	// {
	// 	execve()
	// 	printf("CHILD PROC\n");
	// }
	// else
	// {
	// 	printf("PARENT PROC\n");
	// 	wait(NULL);
	// }

	singleton()->env = env;
	char *ex = search_executable("ls ");
	if (ex)
	{
		ft_exec_cmd(ex);
		free(ex);
	}

	// ft_printf("pwd=[%s]\n", getenv("PWD"));
	
	// ft_getenv(env);
	
	prompt();
	
	return (0);
}
