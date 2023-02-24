#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define     READ_END    0
#define     WRITE_END   1

int main(void)
{
    int fd1[2]; //pipe1
    int fd2[2]; //pipe2
    int status;
    int pid;

    pipe(fd1);
    pid = fork();
    if (pid == 0) //es el hijo
    {
        close(fd1[READ_END]); //cerrar el extremo no usado (lectura)
        dup2(fd1[WRITE_END], STDOUT_FILENO);
        close(fd1[WRITE_END]);

        execlp("/bin/ls", "ls", "-l", NULL);
    }
    else
    {
        close(fd1[WRITE_END]);
        pipe(fd2);
        pid = fork(); //hijo 2
        if (pid == 0)
        {
            close(fd2[READ_END]);
            dup2(fd1[READ_END], STDIN_FILENO);
            close(fd1[READ_END]);
            dup2(fd2[WRITE_END], STDOUT_FILENO);
            close(fd2[WRITE_END]);
            execlp("/bin/grep", "grep", "a", NULL);
        }
        else
        {
            close(fd1[READ_END]);
            close(fd2[WRITE_END]);
            pid = fork();
            if (pid == 0) //hijo 3
            {
                dup2(fd2[READ_END], STDIN_FILENO);
                close(fd2[READ_END]);
                execlp("/usr/bin/wc", "wc", "-l", NULL);
            } 
        }
    }
    close(fd2[READ_END]);
    wait(&status);
    wait(&status);
    wait(&status);

    return (0);
}