#include <libc.h>
// #include "libft.h"
#include <term.h>
#include <curses.h>
#include <errno.h>

// static char *ft_join(char *s1, char *s2)
// {
// 	size_t len;
// 	char *s;
// 	int i;

// 	if (!s1 && !s2)
// 		return (NULL);
// 	len = ((s1)? ft_strlen(s1) : 0) + ((s2)? ft_strlen(s2) : 0);
// 	s = malloc(sizeof(char) * (len + 1));
// 	if (!s)
// 		return (NULL);
// 	i = 0;
// 	while (s1 && s1[i])
// 		*s++ = s1[i++];
// 	i = 0;
// 	while (s2 && s2[i])
// 		*s++ = s2[i++];
// 	*s = '\0';
// 	if (s1)
// 		free(s1);
// 	return (s - len);
// }

// static int ft_newline(char *s)
// {
// 	int i = 0;

// 	if (!s)
// 		return (0);
// 	while (s[i])
// 	{
// 		if (s[i] == '\n')
// 			return (1);
// 		i++;
// 	}
// 	return (0);
// }

// static char *ft_getline(char *s)
// {
// 	char *str;
// 	int i;

// 	if (!s)
// 		return (NULL);
// 	i = 0;
// 	while (s[i] && s[i] != '\n')
// 		i++;
// 	str = malloc(sizeof(char) * i + 1);
// 	if (!str)
// 		return (NULL);
// 	i = 0;
// 	while (s[i] && s[i] != '\n')
// 	{
// 		str[i] = s[i];
// 		i++;
// 	}
// 	str[i] = '\0';
// 	return (str);
// }

// static char *ft_nextline(char *s)
// {
// 	char *str;
// 	int i = 0;
// 	int j = 0;

// 	if (!s)
// 		return (NULL);
// 	while (s[i] && s[i] != '\n')
// 		i++;
// 	if (!s[i])
// 	{
// 		free(s);
// 		return (NULL);
// 	}
// 	str = malloc(sizeof(char) * (ft_strlen(s) - i + 1));
// 	if (!str)
// 		return (NULL);
// 	i++;
// 	while (s[i])
// 	{
// 		str[j] = s[i];
// 		j++;
// 		i++;
// 	}
// 	str[j] = '\0';
// 	free(s);
// 	return (str);
// }

// int				ft_get_next_line(char **line)
// {
// 	static char *s;
// 	int ret = 1;
// 	char buff[1 + 1];

// 	if (!line)
// 		return (-1);
// 	while (!ft_newline(s) && ret != 0)
// 	{
// 		ret = read(STDIN_FILENO, buff, 1);
// 		if (ret < 0)
// 			return (-1);
// 		buff[ret] = '\0';
// 		s = ft_join(s, buff);
// 		if (ft_strstr(s, "\033[A") != NULL)
// 		{
// 			// tputs(tgetstr("cl", NULL), 1, putchar);
// 			dprintf(STDIN_FILENO, "get in gnl\n");

// 			continue ;
// 		}
// 	}
// 	*line = ft_getline(s);
// 	s = ft_nextline(s);
// 	return ((ret) ? 1 : 0);
// }

// int init_term()
// {
//     int ret;
//     char *term_type = getenv("TERM");

//     if (term_type == NULL)
//     {
//         fprintf(stderr, "TERM must be set (see 'env').\n");
//         return (-1);
//     }

//     ret = tgetent(NULL, term_type);

//     if (ret == (-1))
//     {
//         fprintf(stderr, "Could not access to the termcap database..\n");
//         return (-1);
//     }
//     else if (ret == 0)
//     {
//         fprintf(stderr, "Terminal type '%s' is not defined in termcap database (or have too few informations).\n", term_type);
//         return (-1);
//     }

//     return 0;
// }

// int main()
// {
	// int ret = init_term();
	// int r;
	// char *line;

	// if (ret == 0)
	// {
	// 	while (isatty(STDIN_FILENO))
	// 	{
	// 		r = ft_get_next_line(&line);
	// 		// if (line)
	// 		// {
	// 		// 	// printf("line : %s\n", line);
	// 		// 	if (!ft_strcmp(line, "\033[A"))
	// 		// 		printf("up\n");
	// 		// 	else
	// 		// 		printf("Nothing\n");
	// 		// }
	// 		if (r <= 0)
	// 			break ;
	// 	}
	// }

	// char buf[8];

	// ft_bzero(buf, 8);
	// while (1)
	// {
	// 	if (read(0, buf, 8) < 0)
	// 		return 0 ;
	// 	if (!ft_strcmp(buf, "\033[D"))
	// 		tputs(tgetstr("le", NULL), 1, putchar);
	// }

	// struct termios term;
	// int err;

	// if (tcgetattr(STDIN_FILENO, &term) == -1)
	// {
	// 	printf("error\n");
	// 	return (-1);
	// }

	// term.c_cc[VEOF]=(cc_t)0x07;
	// err = tcsetattr(STDIN_FILENO,TCSAFLUSH,&term);

	// if(err==-1 && err == EINTR)
	// {
	// 	perror("Failed to change EOF character");
	// 	return 1;
	// }

	// return 0;

	// char *cl_cap = tgetstr("cl", NULL);
	// tputs (tgetstr("cl", NULL), 1, putchar);
	// dprintf(STDOUT_FILENO, "%s", cl_cap);
// }
