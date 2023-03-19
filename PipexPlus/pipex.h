#ifndef PIPEX_H
# define PIPEX_H

# include "./Libft/libft.h"

/* to write, read, close, access, pipe, dup, dup2, execve, fork */
# include <unistd.h>
# include <sys/types.h>
# include <sys/uio.h>

/* malloc, free, exit */
# include <stdlib.h>

/* open, unlink */
# include <fcntl.h>

/* waitpid, wait */
# include <sys/wait.h>

/* strerror */
# include <string.h>

/*to perror*/
# include <stdio.h>
# include <limits.h>

typedef struct s_pipex
{
	char	**paths;
	char	*command;
	char	**command_args;
	char	*limit;
	int		heredoc;
	int		fd[10][2];
	pid_t	pid[10];
	int		n_cmd;
	char	*infile;
	char	*outfile;
	int		in_fd;
	int		out_fd;
	int		double_out;

}	t_pipex;

#endif
