#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#define FIFONAME1 "./fifo1"
#define FIFONAME2 "./fifo2"
void print_err(char* str){
	perror(str);
	exit(-1);
}
/*封装创建管道+打开管道*/
int create_open_fifo(char *fifoname,int open_mode){
	int ret=0;
	int fd=-1;
	ret=mkfifo(fifoname,0664);
	if(ret==-1 && errno!=EEXIST) print_err("mkfifo fails\n");
	//使用open打开管道文件
	fd=open(fifoname,open_mode);
	if(fd==-1) print_err("open fails\n");
	return fd;
}
/*信号捕获，通信结束，删除管道文件，退出进程*/
void signal_fun(int signo){
	remove(FIFONAME1);
	remove(FIFONAME2);
	exit(-1);
}
int main(void){
	int fd1=-1;
	int fd2=-1;
	//fd1管道去写，fd2管道去读
	fd1=create_open_fifo(FIFONAME1,O_WRONLY);
	fd2=create_open_fifo(FIFONAME2,O_RDONLY);
    char buf[30]={0};
	int ret=fork();
	if(ret>0){//父进程管道2只读
		signal(SIGINT,signal_fun);
	    while(1){
		   bzero(buf,sizeof(buf));
		   read(fd2,buf,sizeof(buf));
		   printf("从管道2读出的数据：%s\n",buf);
	 }
	}
	else if(ret==0){//子进程管道1只写
		while (1){
		   bzero(buf,sizeof(buf));
		   scanf("%s",buf);
		   write(fd1,buf,sizeof(buf));
		}
	}

}


