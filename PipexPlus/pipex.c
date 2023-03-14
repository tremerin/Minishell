#include "pipex.h"

char	**envp_split(char **envp)
{
	int		i;
	char	**paths;

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

void	free_split(char **split)
{
	int		i;

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

char	*get_cmd(char **paths, char *cmd)
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
/* version 1 */
// void	exe_command(t_pipex *pipex, char *cmd, char **envp)
// {
// 	//liberar split del comando anterior si existe
// 	pipex->command_args = ft_split(cmd, ' ');
// 	//ver si es un programa, obtener la ruta absoluta
// 	pipex->command = get_cmd(pipex->paths, pipex->command_args[0]);
// 	execve(pipex->command, pipex->command_args, envp);
//}

/* version 2, comandos y ejecutables */
void	exe_command(t_pipex *pipex, char *cmd, char **envp)
{
	char	*cwd[PATH_MAX];

	getcwd(cwd, sizeof(cwd));
	pipex->command_args = ft_split(cmd, ' ');
	if (ft_strchr(pipex->command_args[0], '/') != 0)
	{
		if (ft_strncmp(pipex->command_args[0], "./", 2) == 0)
		{
			pipex->command = ft_strjoin(cwd, pipex->command_args[0] +1);
		}
		else if (ft_strncmp(pipex->command_args[0], "..", 2) == 0)
		{
			pipex->command = ft_strjoin(cwd, "/");
			pipex->command = ft_strjoin(cwd, pipex->command_args[0]);
		}
		else if (ft_strncmp(pipex->command_args[0], "/", 1) == 0)
		{
			pipex->command = ft_strdup(pipex->command_args[0]);
		}
		pipex->command_args[0] = ft_strdup(ft_strrchr(pipex->command_args[0], '/') + 1);
	}
	else
		pipex->command = get_cmd(pipex->paths, pipex->command_args[0]);
	execve(pipex->command, pipex->command_args, envp);
}

void	close_fd(t_pipex *pipex)
{
	int		i;

	i = 0;
	while (i < pipex->n_cmd -1)
	{
		close(pipex->fd[i][0]);
		close(pipex->fd[i][1]);
		i++;
	}
}

void	dup_fd(t_pipex *pipex, int children)
{
	if (children == 0)
	{
		printf("n_cmd: %d\n", pipex->n_cmd);
		if (pipex->in_fd != -2)
			dup2(pipex->in_fd, STDIN_FILENO);
		if (pipex->out_fd != -2 && pipex->n_cmd == 1)
			dup2(pipex->out_fd, STDOUT_FILENO);
		if (pipex->n_cmd > 1)
			dup2(pipex->fd[0][1], STDOUT_FILENO);
	}
	else if (children == pipex->n_cmd -1)
	{
		if (pipex->out_fd != -2)
			dup2(pipex->out_fd, STDOUT_FILENO);
		dup2(pipex->fd[children -1][0], STDIN_FILENO);
	}
	else
	{
		dup2(pipex->fd[children][1], STDOUT_FILENO);
		dup2(pipex->fd[children - 1][0], STDIN_FILENO);
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
			exe_command(pipex, argv[i + 1], envp);
		}
		i++;
	}
	close_fd(pipex);
	waitpid(-1, NULL, 0);
}

int main(int argc, char **argv, char **envp)
{
	t_pipex		pipex;

	pipex.in_fd = -2;
	pipex.out_fd = -2;
	pipex.double_out = 0; // ( 1 >> ) ( 0 > )
	pipex.outfile = ft_strdup("./file.txt");
	//pipex.infile = ft_strdup("./infile.txt");
	pipex.infile = NULL;
	//pipex.outfile = NULL;
	if (pipex.infile != NULL)
		pipex.in_fd = open(pipex.infile, O_RDONLY);
	if (pipex.outfile != NULL && pipex.double_out)
		pipex.out_fd = open(pipex.outfile, O_APPEND | O_CREAT | O_RDWR, 0664); // O_APPEND escribe desde el final (redireccion >>)
	else
		pipex.out_fd = open(pipex.outfile, O_TRUNC | O_CREAT | O_RDWR, 0664); // O_TRUNC escribe desde el principio (redireccion >)
	pipex.n_cmd = argc -1;
	pipex.paths = envp_split(envp);
	exe_pipex(&pipex, argv, envp);
	if (pipex.outfile != NULL)
		close(pipex.out_fd);
	if (pipex.infile != NULL)
		close(pipex.in_fd);
	free_split(pipex.paths);
	pipex.in_fd = -2;
	pipex.out_fd = -2;
	/* ejecutar programa */
	// char	cwd[PATH_MAX];
	// char	*path = NULL;
	// if (getcwd(cwd, sizeof(cwd)) != NULL)
	// 	printf("getcwd: %s\n", cwd);
	// char **test_cmd = ft_split("./my_prog hola holita adios", ' ');
	// if (test_cmd[0][0] == '.' && test_cmd[0][1] == '/')
	// 	path = ft_strjoin(cwd, test_cmd[0] +1);
	// else if (test_cmd[0][0] == '.' && test_cmd[0][1] == '.')
	// {
	// 	path = ft_strjoin(cwd, "/");
	// 	path = ft_strjoin(path, test_cmd[0]);
	// }
	// else if (test_cmd[0][0] == '/')
	// 	path = ft_strdup(test_cmd[0]);
	// printf("prog path: %s\n", path);
	// test_cmd[0] = ft_strdup(ft_strrchr(test_cmd[0], '/')+1);
	// printf("test_cmd[0]: %s\n", test_cmd[0]);
	// execve(path, test_cmd, envp);
	/* ejecutar programa  v1 */
	//char **test_cmd = ft_split("my_prog hola holita adios", ' ');
	//execve("/home/fgalan-r/Minishell/PipexPlus/../PipexPlus/my_prog", test_cmd, envp);
	return (0);
}

/* main original funcionando */
// int	main(int argc, char **argv, char **envp)
// {
// 	t_pipex		pipex;
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
// 			dup2(fd[0][1], STDOUT_FILENO);
// 			close(fd[0][0]);
// 			close(fd[0][1]);
// 			close(fd[1][0]);
// 			close(fd[1][1]);
// 			exe_command(&pipex, argv[1], envp);
// 		}
// 		pid2 = fork();
// 		if (pid2 == 0)
// 		{
// 			dup2(fd[1][1], STDOUT_FILENO);
// 			dup2(fd[0][0], STDIN_FILENO);
// 			close(fd[0][0]);
// 			close(fd[0][1]);
// 			close(fd[1][0]);
// 			close(fd[1][1]);
// 			exe_command(&pipex, argv[2], envp);
// 		}
// 		pid3 = fork();
// 		if (pid3 == 0)
// 		{
// 			dup2(fd[1][0], STDIN_FILENO);
// 			close(fd[0][0]);
// 			close(fd[0][1]);
// 			close(fd[1][1]);
// 			close(fd[1][0]);
// 			exe_command(&pipex, argv[3], envp);
// 		}
// 		close(fd[0][0]);
// 		close(fd[0][1]);
// 		close(fd[1][0]);
// 		close(fd[1][1]);
// 		//waitpid(pid1, NULL, 0);
// 		//waitpid(pid2, NULL, 0);
// 		//waitpid(pid3, NULL, 0);
// 		waitpid(-1, NULL, 0);
// 		free_split(pipex.paths);
// 	}
// 	return (0);
// }
