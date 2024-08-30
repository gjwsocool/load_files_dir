#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#define FIFONAME "./fifo"
void print_err(char* str){
	perror(str);
	exit(-1);
}
//封装创建管道+打开管道
int create_open_fifo(char *fifoname,int open_mode){
	int ret=0;
	int fd=-1;
	ret=mkfifo(FIFONAME,0664);
	if(ret==-1 && errno!=EEXIST) print_err("mkfifo fails\n");
	/*使用open打开管道文件*/
	fd=open(FIFONAME,open_mode);//以只写方式打开
	if(fd==-1) print_err("open fails\n");
	return fd;
}
/*信号捕获，通信结束，删除管道文件，退出进程*/
void signal_fun(int signo){
	//unlink()
	remove(FIFONAME);
	exit(-1);
}
int main(void){
	signal(SIGINT,signal_fun);//捕获：必须在create_open_fife前
	/*因为读端堵塞，以只写方式打开*/
	int fd=create_open_fifo(FIFONAME,O_WRONLY);
	
	while(1){
		char buf[30]={0};
		bzero(buf,sizeof(buf));
		scanf("%s",buf);
		write(fd,buf,sizeof(buf));
	}

}


