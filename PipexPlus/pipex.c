#include "pipex.h"

char **envp_split(char **envp)
{
	int     i;
	char    **paths;

	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], "PATH", 4) == 0)
		{
			paths = ft_split(envp[i] + 5, ':');
			i = 0;
			while (paths[i] != NULL)
			{
				paths[i] = ft_strjoin(paths[i], "/");
				i++;
			}
			return (paths);
		}
		i++;
	}
	return (NULL);
}

void free_split(char **split)
{
	int     i;

	i = 0;
	while (split[i] != NULL)
		i++;
	while (i >= 0)
	{
		free(split[i]);
		i--;
	}
	free(split);
}

char *get_cmd(char **paths, char *cmd)
{
	char	*command;

	while (*paths)
	{
		command = ft_strjoin(*paths, cmd);
		if (access(command, 0) == 0)
			return (command);
		free(command);
		paths++;
	}
	return (NULL);
}

void    exe_command(t_pipex pipex, char *cmd, char **envp)
{
	//printf("exe\n");
	pipex.command_args = ft_split(cmd, ' ');
	pipex.command = get_cmd(pipex.paths, pipex.command_args[0]);
	execve(pipex.command, pipex.command_args, envp);
}

void	close_fd(t_pipex *pipex)
{
	int		i;

	i = 0;
	while (0 < pipex->n_cmd -1)
	{
		close(pipex->fd[i][0]);
		close(pipex->fd[i][1]);
	}
}

void	dup_fd(t_pipex *pipex, int children)
{
	if (children == 0)
	{
		dup2(pipex->fd[0][1], STDOUT_FILENO);
	}
	else if (children == pipex->n_cmd -1)
	{
		dup2(pipex->fd[children][0], STDIN_FILENO);
	}
	else
	{
		dup2(pipex->fd[children][1], STDOUT_FILENO);
		dup2(pipex->fd[children -1][0], STDIN_FILENO);
	}
}

void	exe_pipex(t_pipex *pipex, char **argv, char **envp)
{
	int		i;

	i = 0;
	while (i < pipex->n_cmd -1)
	{
		pipe(pipex->fd[i]);
		i++;
	}
	i = 0;
	while (i < pipex->n_cmd)
	{
		pipex->pid[i] = fork();
		if (pipex->pid[i] == 0)
		{
			dup_fd(pipex, i);
			close_fd(pipex);
			exe_command(*pipex, argv[i], envp);
		}
		i++;
	}
	close_fd(pipex);
	waitpid(-1, NULL, 0);
}

int main(int argc, char **argv, char**envp)
{
	t_pipex     pipex;

	pipex.n_cmd = argc -1;
	pipex.paths = envp_split(envp);
	exe_pipex(&pipex, argv, envp);
	free_split(pipex.paths);
	
	return (0);
}





/* main original funcionando */
// int main(int argc, char **argv, char**envp)
// {
// 	t_pipex     pipex;
// 	pid_t		pid1;
// 	pid_t		pid2;
// 	pid_t		pid3;
// 	int			fd[10][2];

// 	if (argc > 1)
// 	{
// 		pipex.paths = envp_split(envp);

// 		pipe(fd[0]);
// 		pipe(fd[1]);

		
// 		pid1 = fork();
		
// 		if (pid1 == 0)
// 		{
// 			//printf("pid 1, pid: %d\n", getpid());
// 			close(fd[0][0]);
// 			close(fd[1][0]);
// 			close(fd[1][1]);
// 			dup2(fd[0][1], STDOUT_FILENO);
// 			close(fd[0][1]);
// 			exe_command(pipex, argv[1], envp);
// 		}	
		
// 		pid2 = fork();

// 		if (pid2 == 0)
// 		{
// 			//printf("pid 2 pid: %d\n", getpid());
			
// 			close(fd[0][1]);
// 			close(fd[1][0]);
// 			dup2(fd[1][1], STDOUT_FILENO);
// 			dup2(fd[0][0], STDIN_FILENO);
// 			close(fd[0][0]);
// 			close(fd[1][1]);
// 			exe_command(pipex, argv[2], envp);
// 		}

// 		pid3 = fork();

// 		if (pid3 == 0)
// 		{
// 			//printf("pid 3 pid: %d\n", getpid());
// 			close(fd[0][0]);
// 			close(fd[0][1]);
// 			close(fd[1][1]);
// 			dup2(fd[1][0], STDIN_FILENO);
// 			close(fd[1][0]);	
// 			exe_command(pipex, argv[3], envp);
// 		}	

// 		close(fd[0][0]);
// 		close(fd[0][1]);
// 		close(fd[1][0]);
// 		close(fd[1][1]);
// 		waitpid(pid1, NULL, 0);
// 		waitpid(pid2, NULL, 0);
// 		waitpid(pid3, NULL, 0);	
		
// 		free_split(pipex.paths);
// 	}
// 	return (0);
// }