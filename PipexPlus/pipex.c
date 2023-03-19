#include "pipex.h"

void	heredoc(t_pipex *pipex)
{
	char	*buf;

	pipex->in_fd = open(pipex->infile, O_TRUNC | O_CREAT | O_WRONLY, 0644);
	while (1)
	{
		write(1, "heredoc> ", 9);
		buf = get_next_line(0);
		if (!ft_strncmp(pipex->limit, buf, ft_strlen(pipex->limit)))
			break ;
		write(pipex->in_fd, buf, ft_strlen(buf));
		free(buf);
	}
	free(buf);
	close(pipex->in_fd);
}

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
	char	*cwd;

	//printf("check\n");
	//cwd = NULL;
	//cwd = ft_strdup(getcwd(cwd, sizeof(1024)));
	cwd = ft_strdup("/home/fgalan-r/Minishell/PipexPlus");
	//printf("cwd: %s\n", cwd);
	pipex->command_args = ft_split(cmd, ' ');
	if (ft_strchr(pipex->command_args[0], '/'))
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
		//printf("n_cmd: %d\n", pipex->n_cmd);
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
	//pipex.outfile = ft_strdup("./file.txt");
	//pipex.infile = ft_strdup("./infile.txt");
	pipex.heredoc = 1;
	pipex.limit = ft_strdup("fin");
	if (pipex.heredoc)
	{
		pipex.infile = ft_strdup("./temp.text");
		heredoc(&pipex);
	}
	//pipex.infile = NULL;
	pipex.outfile = NULL;
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
	return (0);
}
