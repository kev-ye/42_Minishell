#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int fd[2];
    int pid;
    pipe(fd);
    int rpipe = fd[0];
    int wpipe = fd[1];
    pid = fork();
    if (pid == 0)
    {   
        /* 子进程关掉读的那端，只用写的一端 */
        close(rpipe); 

        /* 把标准输入指向pipe的写的一端 */
        dup2(wpipe, STDOUT_FILENO);

        /* 执行pwd */
        execl("/bin/pwd",".",NULL);

        /* 嗯 */
        exit(0);
    }   
    else
    {   
        /* 父进程关掉写的那端，只用读的一端 */
        close(wpipe);

        printf("begin parent process\n");
        char readbuffer[1024];

        /* 从读的这端读出pwd的输出 */
        int nbytes = read(rpipe, readbuffer, sizeof(readbuffer));

        printf("Received string: %s | %d\n", readbuffer, nbytes);
        printf("end parent process\n");

        wait(&pid);
    }   
    return 0;
}