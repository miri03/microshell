/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <meharit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/12 13:15:02 by meharit           #+#    #+#             */
/*   Updated: 2023/07/12 14:19:46 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0



int error_msg(char *msg, int ext, int stats)
{
    while (*msg)
    {
        write(2, msg, 1);
        msg++;
    }
    if (ext)
        exit(stats);
    return (1);
    
}

int ft_cd(char **argv, int i, int cmd)
{
    if (cmd - i != 2)
        return (error_msg("error: cd: bad arguments\n", 1, FALSE));
    if (chdir(argv[cmd]) == -1)
        return (error_msg("error: cd: cannot change directory to ", 1, FALSE), error_msg(argv[cmd+1], 1, FALSE), error_msg("\n", 1, FALSE));
	return (0);
}

int pipe_exist(char **argv, int i, int cmd)
{
    if (argv[i] && !strcmp(argv[i], "|"))
        return (1);
    if (argv[cmd] && !strcmp(argv[cmd], "|"))
        return (-1);
    return (0);
}

int exec(char **argv, int i, int cmd, char **path)
{
    int w_pipe;
    int pp[2];
    int pid;
    int status;

    w_pipe = pipe_exist(argv, i, cmd);
    if (w_pipe == -1)
        return (0);
    if (w_pipe && pipe(pp) == -1)
        return (error_msg("error: fatal\n", TRUE, 1));
    pid = fork();
    if (pid == 0)
    {
        argv[i] = NULL;
        if (w_pipe && (dup2(pp[1], 1) == -1 || close(pp[1]) == -1 || close(pp[0]) == -1))
            return (error_msg("error: fatal\n", TRUE, 1));
        if (execve(argv[cmd], &argv[cmd], path) == -1)
            return (error_msg("error: cannot execute ", 127, FALSE), error_msg(argv[cmd], 127, FALSE), error_msg("\n", 127, FALSE));
    }
    waitpid(pid, &status, 0);
    if (w_pipe && (dup2(pp[0], 0) == -1 || close(pp[1]) == -1 || close(pp[0]) == -1))
        return (error_msg("exec", FALSE, 1));
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    return (status);
}

int main(int argc, char **argv, char **path)
{
    int i;
    int cmd;
    int status;

    status = 0;
    i = 1;
    if (argc > 1)
    {
        while (argv[i])
        {
            cmd = i;
            while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
                i++;
            if (!strcmp(argv[cmd], "cd"))
                status = ft_cd(argv, i, cmd);
            else
                status = exec(argv, i , cmd, path);
            if (argv[i])
                i++;
        }
    }
    return (status);
}