/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_parser_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 13:53:35 by kaye              #+#    #+#             */
/*   Updated: 2021/06/19 17:27:27 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int trunc_fd = O_WRONLY | O_TRUNC | O_CREAT;
static int append_fd = O_WRONLY | O_APPEND | O_CREAT;

void parser_output_fd(t_list *cmd, int *fd_output, int flag_is)
{
    if (flag_is == F_TRUNC)
    {
        if (*fd_output != -1)
            close(*fd_output);
        *fd_output = open(((t_cmd *)cmd->content)->args[0], trunc_fd, 0666);
    }
    else if (flag_is == F_APPEND)
    {
        if (*fd_output != -1)
            close(*fd_output);
        *fd_output = open(((t_cmd *)cmd->content)->args[0], append_fd, 0666);
    }
    if (*fd_output != -1)
        dup2(*fd_output, STDOUT_FILENO);
}

// t_list *parser_input_fd(t_list *cmd, int *fd_input, int flag_is, char **input_str)
// {
//     int ctrld;

//     if (flag_is == F_INPUT)
//     {
//         *fd_input = open(((t_cmd *)cmd->content)->args[0], O_RDWR);
//         if (*fd_input == -1)
//         {
//             ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)cmd->content)->args[0], strerror(errno));
// 			exit(LRV_GENERAL_ERROR);
//         }
//         else
//             dup2(*fd_input, STDIN_FILENO);
//     }
//     else if (flag_is == F_DINPUT)
//     {
//         *fd_input = open(TMP_FD, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
// 		if (*fd_input == -1)
// 		{
// 			ft_dprintf(STDERR_FILENO, "open for double input crash\n");
// 			exit(LRV_GENERAL_ERROR);
// 		}
//         else
//         {
//             ctrld = 0;
//             while (1)
//             {
//                 singleton()->rl_lvl = 2;
//                 *input_str = readline("> ");
//                 if (*input_str && !ft_strcmp(*input_str, ((t_cmd *)cmd->content)->args[0]))
//                 {
//                     free(*input_str);
//                     if (((t_cmd *)cmd->content)->status_flag & FLG_DINPUT)
//                     {
//                         close(*fd_input);
//                         *fd_input = open(TMP_FD, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
//                         if (*fd_input == -1)
//                         {
//                             ft_dprintf(STDERR_FILENO, "open for double input crash\n");
//                             exit(LRV_GENERAL_ERROR);
//                         }
//                         cmd = cmd->next;
//                         continue ;
//                     }
//                     else
//                         break ;
//                 }
//                 else if (!*input_str)
//                 {
//                     if (flag_check(cmd) == FLG_DINPUT)
//                         ctrld = 1;
//                     break ;
//                 }
//                 if (*input_str)
//                     ft_putendl_fd(*input_str, *fd_input);
//             }
//             if (*fd_input != -1)
//                 close(*fd_input);
//             if (!ctrld)
//             {
//                 *fd_input = open(TMP_FD, O_RDONLY);
//                 if (*fd_input == -1)
//                 {
//                     ft_dprintf(STDERR_FILENO, "open for double input crash\n");
//                     exit(LRV_GENERAL_ERROR);
//                 }
//                 dup2(*fd_input, STDIN_FILENO);
//             }
//         }
//     }
//     return (cmd);
// }

char *get_tmp_fd(int i)
{
    char *old_name;
    char *new_name;
    char *fd_nbr;
    int fd;

    fd = -1;
    while (1)
    {
        fd_nbr = ft_itoa(i);
        old_name = malloc(sizeof(char) * ft_strlen(TMP_FD) + 1);
        if (!old_name)
            return (NULL);
        ft_strcpy(old_name, TMP_FD);
        new_name = ft_strjoin(old_name, fd_nbr);
        free(old_name);
        free(fd_nbr);
        fd = open(new_name, O_RDWR);
        if (fd != -1)
        {
            close(fd);
            return (new_name);
        }
        else
            ++i;
    }
    return (NULL);
}

t_list *parser_input_fd(t_list *cmd, int *fd_input, int flag_is, char **input_str)
{
    char *tmp_fd_name;
    int i;

    (void)input_str;
    tmp_fd_name = NULL;
    i = 1;
    if (flag_is == F_INPUT)
    {
        *fd_input = open(((t_cmd *)cmd->content)->args[0], O_RDWR);
        if (*fd_input == -1)
        {
            ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)cmd->content)->args[0], strerror(errno));
			exit(LRV_GENERAL_ERROR);
        }
        else
            dup2(*fd_input, STDIN_FILENO);
    }
    else if (flag_is == F_DINPUT)
    {
        tmp_fd_name = get_tmp_fd(i);
        *fd_input = open(tmp_fd_name, O_RDWR);
        free(tmp_fd_name);
		if (*fd_input == -1)
		{
			ft_dprintf(STDERR_FILENO, "open for double input crash parser\n");
			exit(LRV_GENERAL_ERROR);
		}
        else
        {
            dup2(*fd_input, STDIN_FILENO);
        }
    }
    return (cmd);
}

void redir_parser2(t_list *cmd, int *fd_input, int *fd_output)
{
    int first;
    int flag_is;
    char *input_str;
    
    flag_is = 0;
    first = 1;
    input_str = NULL;
    while (cmd)
    {
        if (first == 1)
            first = 0;
        else if (!first && (flag_is == F_APPEND || flag_is == F_TRUNC))
            parser_output_fd(cmd, fd_output, flag_is);
        else if (!first && (flag_is == F_INPUT || flag_is == F_DINPUT))
            cmd = parser_input_fd(cmd, fd_input, flag_is, &input_str);
        flag_is = check_for_next(cmd);
        if (!flag_is)
            return ;
        cmd = cmd->next;
    }
}