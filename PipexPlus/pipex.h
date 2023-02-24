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

typedef struct s_pipex
{
    char    **paths;
    int     fd[2];
    char    *command;
    char    **command_args;
}   t_pipex;

# endif