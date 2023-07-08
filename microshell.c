/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 18:53:35 by meharit           #+#    #+#             */
/*   Updated: 2023/07/08 15:40:41 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

int	error_mess(char *msg, int status, int st_exit)
{
	while (msg && *msg)
	{
		write(2, msg, 1);
		msg++;
	}
	if (st_exit == TRUE)
		exit(status);
	return (1);
}

int	ft_cd(char **argv, int i, int cmd)
{
	if (i - cmd != 2)
		return (error_mess("error: cd: bad arguments\n", 1, FALSE));
	if (chdir(argv[cmd+1]) == -1)
		return (error_mess("error: cd: cannot change directory to ", 1, FALSE), error_mess(argv[cmd+1], 1, FALSE), error_mess("\n", 1, FALSE));
	return (0);
}

int	pipe_next(char **argv, int i, int cmd)
{
	if (argv[i] && !strcmp(argv[i], "|"))
		return (1);
	if (argv[cmd] && !strcmp(argv[cmd], ";"))
		return (-1);
	return (0);
}

int	exec(char **argv, char **path, int i, int cmd)
{
	int	pp[2];
	int	w_pipe;
	int	pid;
	int	status;

	w_pipe = pipe_next(argv, i, cmd);
	if (w_pipe == -1)
		return (0);
	if (w_pipe && pipe(pp) == -1)
		return (error_mess("error: fatal\n", 1, TRUE));
	pid = fork();
	if (pid == 0)
	{
		argv[i] = NULL;
		if (w_pipe && (dup2(pp[1], 1) == -1 || close(pp[0]) == -1 || close(pp[1]) == -1))
			return(error_mess("error: fatal\n", 1, TRUE));
		if (execve(argv[cmd], &argv[cmd], path) == -1)
			return (error_mess("error: cannot execute ", 127, FALSE), error_mess(argv[cmd], 127, FALSE), error_mess("\n", 127, FALSE));
	}
	waitpid(pid, &status, 0);
	if (w_pipe && (dup2(pp[0], 0) == -1 || close(pp[0]) == -1 || close(pp[1]) == -1))
		error_mess("error: fatal\n", 1, TRUE);
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	return (status);
}

int main(int argc, char **argv, char **path)
{
	int i;
	int	pipe;
	int	status;
	int	cmd;

	i = 1;
	status = 0;
	if (argc > 1)
	{
		while (argv[i])
		{
			cmd = i;
			pipe = 0;
			while (argv[i] && (strcmp(argv[i], "|") && strcmp(argv[i], ";")))
				i++; //index of "|" or ";" or NULL
			if (!(strcmp(argv[cmd], "cd")))
				status = ft_cd(argv, i,cmd);
			else
				status = exec(argv, path, i, cmd);
			if (argv[i])
				i++;
		}
	}
	return (status);
}

