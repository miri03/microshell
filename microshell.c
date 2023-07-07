/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meharit <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 18:53:35 by meharit           #+#    #+#             */
/*   Updated: 2023/07/07 19:11:02 by meharit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void	error_mess(char *msg, int status)
{
	while (*msg)
	{
		write(2, msg, 1);
		msg++;
	}
	if (status > 0)
		exit(status);
}

void	ft_cd(char **argv, int i, int cmd)
{
	printf("----> i %d\n", i);
	if (i >= 4)
	{
		error_mess("error: cd: bad arguments\n", -1);
		return;
	}
	if (chdir(argv[cmd+1]) == -1)
	{
		error_mess("error: cd: cannot change directory to ", -1);
		error_mess(argv[cmd+1], -1);
		error_mess("\n", -1);
	}
}

int	pipe_next(char **argv, int i)
{
//	printf("i -->%d %s\n", i, argv[i]);
	if (argv[i] && !strcmp(argv[i], "|"))
		return (1);
//	printf("9alwa\n");
	return (0);
}

int	exec(char **argv, char **path, int i, int cmd)
{
	int	pp[2];
	int	w_pipe;
	int	pid;
	int	status;

	w_pipe = pipe_next(argv, i);
//	printf("pp-> %d\n", w_pipe);
	if (w_pipe && pipe(pp) == -1)
		error_mess("error: fatal\n", 1);
	pid = fork();
	if (pid == 0)
	{
//		printf("before = %s   ", argv[i]);
		argv[i] = NULL;
//		printf("after = %s\n", argv[i]);
		if (w_pipe && (dup2(pp[1], 1) == -1 || close(pp[0]) == -1 || close(pp[1]) == -1))
			error_mess("error: fatal\n", 1);
//		dprintf(2, "cmd = %d argv[cmd] = %s\n", cmd, argv[cmd]);
		if (execve(argv[cmd], &argv[cmd], path) == -1)
		{
			error_mess("error: cannot execute ", -1);
			error_mess(argv[cmd], -1);
			error_mess("\n", 127);
		}
	}
	waitpid(pid, &status, 0);
	if (w_pipe && (dup2(pp[0], 0) == -1 || close(pp[0]) == -1 || close(pp[1]) == -1))
		error_mess("error: fatal\n", 1);
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
	if (argc > 1)
	{
		while (argv[i])
		{
			cmd = i;
			pipe = 0;
			while (argv[i] && (strcmp(argv[i], "|") && strcmp(argv[i], ";")))
				i++; //index of "|" or ";" or NULL
			if (!(strcmp(argv[cmd], "cd")))
				ft_cd(argv, i,cmd);
			else
				status = exec(argv, path, i, cmd);
			if (argv[i])
				i++;
		}
	}
	return (0);
}

