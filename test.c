#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define SIZE 128				//缓存区大小	

void read_from_pipe(int fd[],int err_fd[]);	//从管道读数据
void write_to_pipe(int fd[],int err_fd[]);	//向管道写数据
void handle(int signo);			//信号处理函数
int main()
{
	int fd[2];//文件描述符数组，数据管道，传输shell命令执行返回的结果
	int err_fd[2];//文件描述符数组，错误管道，传输shell命令执行产生的错误信息
	int ret;
	int err_ret;
	pid_t pid;
	char s[3];
	do
	{
		ret=pipe(fd);//创建无名管道fd[0]读端,fd[1]写端
		if(ret!=0)
		{
			perror("pipe error");
			exit(1);
		}
		err_ret=pipe(err_fd);
		if(err_ret!=0)
		{
			perror("pipe error");
			exit(1);
		}
		signal(SIGCHLD,handle);//子进程退出信号捕捉函数
		pid=fork();//创建进程
		if(pid==0)//子进程
		{	
			write_to_pipe(fd,err_fd);	
			exit(1);
		}
		else if(pid>0)//父进程
		{	
			wait(NULL);
			read_from_pipe(fd,err_fd);
		}
		else
		{
			perror("fork error");
			exit(1);
		}
		printf("<y or n>--->");
		fgets(s,3,stdin);
	}while(strcmp("y\n",s)==0);
	return 0;
}
void read_from_pipe(int fd[],int err_fd[])
{
    // int test_fd;
    // test_fd = open((, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	char buf_read[SIZE];
	memset(buf_read,'\0',SIZE);
	char buf_err[SIZE];
	memset(buf_err,'\0',SIZE);
	close(fd[1]);
	close(err_fd[1]);
	if(read(fd[0],buf_read,SIZE)>=0 && read(err_fd[0],buf_err,SIZE)==0)
	{
		printf("命令正确\n");
		if(strlen(buf_read)==0)
		{
			printf("此操作没有数据可写\n");
		}
		else
		{
			printf("data:\n");
			printf("%s",buf_read);
		}
		printf("=========================\n");
	}
	else
	{	
		printf("命令错误\n");	
		printf("error info:\n");
		printf("%s",buf_err);
		printf("=========================\n");
	}
	close(fd[0]);
	close(err_fd[0]);
}
void write_to_pipe(int fd[],int err_fd[])
{
	char buf_write[SIZE];
	int len;int ret;
	memset(buf_write,'\0',SIZE);//分配内存空间
	close(fd[0]);//关闭数据管道读端
	close(err_fd[0]);//关闭错误管道读端
	printf("input:");
	fflush(stdout);//缓存清空（注意！！！）
	dup2(fd[1],1);//标准输出重定向到数据管道写端
	dup2(err_fd[1],2);//标准错误重定向到错误管道写端
	fgets(buf_write,SIZE,stdin);
	len=strlen(buf_write);
	buf_write[len-1]='\0';
	ret=execlp("sh","sh","-c",buf_write,NULL);//执行shell命令
	if(ret==-1)
	{
		perror("execlp error");
		exit(1);
	}
	close(fd[1]);
	close(err_fd[1]);
}
void handle(int signo)
{
	if(signo==SIGCHLD)
	printf("子进程退出\n");
}