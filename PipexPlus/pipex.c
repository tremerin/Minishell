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
	pipex.command_args = ft_split(cmd, ' ');
	pipex.command = get_cmd(pipex.paths, pipex.command_args[0]);
	execve(pipex.command, pipex.command_args, envp);
}

typedef struct s_fd
{
	int	fd[2];
}	t_fd;


int main(int argc, char **argv, char**envp)
{
	t_pipex     pipex;
	pid_t		pid1;
	pid_t		pid2;
	pid_t		pid3;
	int			fd[10][2];

	if (argc > 1)
	{
		pipex.paths = envp_split(envp);

		pipe(fd[0]);
		pid1 = fork();
		if (pid1 == 0)
		{
			dup2(fd[0][1], STDOUT_FILENO);
			close(fd[0][0]);
			exe_command(pipex, argv[1], envp);
		}
		waitpid(pid1, NULL, 0);
		pipe(fd[1]);
		pid2 = fork();
		if (pid2 == 0)
		{
			dup2(fd[0][0], STDIN_FILENO);
			close(fd[0][1]);
			dup2(fd[1][1], STDOUT_FILENO);
			close(fd[1][0]);
			exe_command(pipex, argv[2], envp);
		}
		waitpid(pid2, NULL, 0);
		pid3 = fork();
		if (pid3 == 0)
		{
			dup2(fd[1][0], STDIN_FILENO);
			close(fd[1][1]);
			exe_command(pipex, argv[3], envp);
		}
		
		waitpid(pid3, NULL, 0);	
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		
		free_split(pipex.paths);
	}
	return (0);
}