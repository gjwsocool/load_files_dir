### I/O多路复用

> 又叫：I/O多路转接

I/O操作：

> 通信双方都用socket文件描述符fd，fd是在内核中开辟的`读写缓冲区`;而I/O就是对缓冲区的操作

```ABAP
I/O 多路复用使得程序能同时监听多个文件描述符，能够提高程序的性能
```

##### Linux 下实现 I/O 多路复用的系统调用主要有 select、poll 和 epoll

-----

##### 四种网络I/O模型：

+ 阻塞IO模型

+ 非阻塞IO模型

> 阻塞和非阻塞：
>
> 场景：用户进程系统调用内核的I/O
>
> 阻塞：内核的I/O操作数据准备好后，才会返回到用户进程，此时用户进程一直阻塞
>
> 非阻塞：内核只有在数据准备好后，才会完成I/O操作；否则会在系统调用时候给用户进程发送错误，用户进程想要I/O操作需要再次发送系统调用请求

+ IO多路复用模型
+ 异步IO模型

> 用户进程向系统内核发送I/O请求，然后内核会立即给用户进程返回值，用户进程不会阻塞去做其他事；而系统内核会等待数据的准备，并把数据拷贝到用户内存。I/O完成后，内核会给用户进程一个信号，告知其I/O操作已完成

![image-20230324172155137](/home/guojiawei/.config/Typora/typora-user-images/image-20230324172155137.png)



![image-20230324172315476](/home/guojiawei/.config/Typora/typora-user-images/image-20230324172315476.png)

> NIO模型
>
> 优点: 提高了程序的执行效率
> 缺点: 需要占用更多的CPU和系统资源

----

--通过内核把多路变成一路，所谓多路就是检测每一个客户端的读写缓冲区的数据

#### select/poll----I/O多路转接技术

![image-20230324172604952](/home/guojiawei/.config/Typora/typora-user-images/image-20230324172604952.png)

> 内核只会告诉数据到达的个数，但不会告知具体的数据对应的fd，需要遍历

#### epoll---I/O多路转接复用

![image-20230324172816018](/home/guojiawei/.config/Typora/typora-user-images/image-20230324172816018.png)

> 告知数据以及对应的fd

---

### select

思想：

```ABAP
1.首先构造一个关于文件描述符的列表，将要监听的文件描述符添加到列表中
2.其次调用一个系统函数，监听列表文件描述符的数据，直到有一个和多个进行I/O操作才返回
（内核完成，这个系统函数阻塞）
3.最后返回值会告诉用户进程有多个描述符需要进行I/O操作
```

```c
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
#nfds：文件描述符范围--最大值+1
#readfds:委托内核检测文件描述符的读的集合，即内核检测读缓冲区是否有数据
#writefds : 要检测的文件描述符的写的集合，写缓冲区不满就可以写
#exceptfds : 检测发生异常的文件描述符的集合
#timeout : 设置的超时时间
/*对fd的二进制位来检测*/
//-1失败，n为发生变化的
```

![image-20230324175838063](/home/guojiawei/.config/Typora/typora-user-images/image-20230324175838063.png)

![image-20230324175849940](/home/guojiawei/.config/Typora/typora-user-images/image-20230324175849940.png)

![image-20230324175858098](/home/guojiawei/.config/Typora/typora-user-images/image-20230324175858098.png)



---

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
int main(){
    int lfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveAddr;
    serveAddr.sin_family=AF_INET;
    serveAddr.sin_port=htons(9999);
    serveAddr.sin_addr.s_addr=INADDR_ANY;
    bind(lfd,(struct sockaddr *)&serveAddr,sizeof(serveAddr));
    listen(lfd,128);
    /*IO复用多路技术*/
    //创建一个存放文件描述符的集合
    fd_set rdset,tmp;
    FD_ZERO(&rdset);//fd_set一共有1024 bit, 全部初始化为0
    FD_SET(lfd,&rdset);//将参数文件描述符fd对应的标志位，设置为1
    int maxfd=lfd;
    while(1){
        tmp=rdset;
        //调用select，让内核找出有数据的文件描述符
        int ret=select(maxfd+1,&tmp,NULL,NULL,NULL);
        if(ret==-1){
            perror("select fails\n");
        }
        else if(ret==0){
            continue;
        }
        else if(ret>0){//检测到了
            if(FD_ISSET(lfd,&tmp)){
                //客户端连接
                struct sockaddr_in cliaddr;
                int len=sizeof(cliaddr);
                int cfd=accept(lfd,(struct sockaddr*)&cliaddr,&len);
                //加入集合
                FD_SET(cfd,&rdset);
                maxfd=maxfd >cfd?maxfd:cfd;
            }
            for(int i=lfd+1;i<=maxfd;i++){
                //检测用户进程的文件描述符集合的1
                if(FD_ISSET(i,&tmp)){
                    char buf[1024]={0};
                    int len=read(i,buf,sizeof(buf));
                    //read判断
                    if(len==-1){
                       perror("read");
                       exit(-1);
                    }
                    else if(len ==0){
                       printf("client closed\n");
                       close(i);
                       FD_CLR(i,&rdset);
                    }
                    else if(len>0){
                       printf("buf=%s\n",buf);
                       write(i,buf,strlen(buf)+1);
                    }
                }
            }
        }
    }
    close(lfd);
    return 0;
}
```

客户端：

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
int main(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        perror("socket");
        exit(-1); 
    }
    //连接
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr.s_addr);
    serveraddr.sin_port=htons(9999);
    int ret=connect(fd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(ret==-1){
        perror("connect");
        exit(-1);
    }
    //通信
    char recvBuf[1024]={0};
    int i=0;
    while(1){
        sprintf(recvBuf,"data:%d\n",i++);
        //服务端写数据
        write(fd,recvBuf,strlen(recvBuf));
         int len=read(fd,&recvBuf,sizeof(recvBuf));
                if(len==-1){
                    perror("read");
                    exit(-1);
                }
                else if(len>0){
                printf("data=%s\n",recvBuf);
                }
                else{
                    printf("client closed\n");
                    break;
                }
        sleep(1);
    }
    close(fd);
    return 0;
}
```

重点

```C
// 创建一个fd_set的集合，存放的是需要检测的文件描述符
    fd_set rdset, tmp;
    /*
    tmp是内核操作的，rdset只能：添加新描述符,去清空
    */
    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);
    int maxfd = lfd;
    while(1) {
        tmp = rdset;
        // 调用select系统函数，让内核帮检测哪些文件描述符有数据
        int ret = select(maxfd + 1, &tmp, NULL, NULL, NULL);
        if(ret == -1) {
            perror("select");
            exit(-1);
        } else if(ret == 0) {
            continue;
        } else if(ret > 0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if(FD_ISSET(lfd, &tmp)) {//判断fd对应的标志位是0还是1
                // 表示有新的客户端连接进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                // 将新的文件描述符加入到集合中
                FD_SET(cfd, &rdset);
                // 更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }
            //用户做的事
            for(int i = lfd + 1; i <= maxfd; i++) {
                if(FD_ISSET(i, &tmp)) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(i, buf, sizeof(buf));
                    if(len == -1) {
                        perror("read");
                        exit(-1);
                    } else if(len == 0) {
                        printf("client closed...\n");
                        close(i);
                        FD_CLR(i, &rdset);
                    } else if(len > 0) {
                        printf("read buf = %s\n", buf);
                        write(i, buf, strlen(buf) + 1);
                    }
                }
            }

        }

    }
```

![image-20230324221603373](/home/guojiawei/.config/Typora/typora-user-images/image-20230324221603373.png)

![image-20230324221644116](/home/guojiawei/.config/Typora/typora-user-images/image-20230324221644116.png)

### poll

```C
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
int fd;/* 委托内核检测的文件描述符 */
short events;/* 委托内核检测文件描述符的什么事件 */
short revents;/* 文件描述符实际发生的事件 */
};
//nfds : 这个是第一个参数数组中最后一个有效元素的下标 + 1
```

![image-20230324222215948](/home/guojiawei/.config/Typora/typora-user-images/image-20230324222215948.png)

```c
 // 初始化检测的文件描述符数组
    struct pollfd fds[1024];
    for(int i = 0; i < 1024; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;
    int nfds = 0;

    while(1) {
        // 调用poll系统函数，让内核帮检测哪些文件描述符有数据
        int ret = poll(fds, nfds + 1, -1);
        if(ret == -1) {
            perror("poll");
            exit(-1);
        } else if(ret == 0) {
            continue;
        } else if(ret > 0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if(fds[0].revents & POLLIN) {
                // 表示有新的客户端连接进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                // 将新的文件描述符加入到集合中
                for(int i = 1; i < 1024; i++) {
                    if(fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                // 更新最大的文件描述符的索引
                nfds = nfds > cfd ? nfds : cfd;
            }
            for(int i = 1; i <= nfds; i++) {
                if(fds[i].revents & POLLIN) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(fds[i].fd, buf, sizeof(buf));
                    if(len == -1) {
                        perror("read");
                        exit(-1);
                    } else if(len == 0) {
                        printf("client closed...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    } else if(len > 0) {
                        printf("read buf = %s\n", buf);
                        write(fds[i].fd, buf, strlen(buf) + 1);
                    }
                }
            }
        }
    }
```

















