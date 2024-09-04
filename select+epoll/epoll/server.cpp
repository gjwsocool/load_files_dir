#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/epoll.h>
#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
using namespace std;

#define BUFFSIZE 1024

// 出错调用函数
static void error_handle(std::string opt, std::string message)
{
    // 根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    // 判断命令行参数合法性
    if (argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " <port>" << std::endl;
        exit(1);
    }

    // 创建socket套接字
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        error_handle("socket", "socket() error.");
    }

    // 绑定
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    int re = bind(server_fd, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if (re < 0)
    {
        error_handle("bind", "bind() error.");
    }

    // 监听TCP端口号
    re = listen(server_fd, 10);
    if (re < 0)
    {
        error_handle("listen", "listen() error.");
    }
    /*创建树*/
    int epfd = epoll_create(1);
    /*将server_fd上树*/
    struct epoll_event ev, ev_ret[1024];
    ev.data.fd = server_fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    /*while监听*/
    while (1)
    {
        // n是返回的个数
        int n = epoll_wait(epfd, ev_ret, 1024, -1);
        if (n < 0)
        { // 出错
            perror("");
            break;
        }
        else if (n == 0)
        { // 无变化
            continue;
        }
        else
        { // 监听成功，有文件描述符变化
            // 遍历ev_ret,只看变换的
            for (int i = 0; i < n; i++)
            {
                // 当前文件描述符是server_fd
                if (ev_ret[i].data.fd == server_fd && ev_ret[i].events & EPOLLIN)
                {
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    int cfd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
                    cout << "Accept New Client : " << inet_ntoa(client_addr.sin_addr);
                    cout << " , port : " << ntohs(client_addr.sin_port) << endl;
                    //设置cfd为非阻塞，防止循环读一直阻塞
                    int flag=fcntl(cfd, F_GETFL);//获得cfd的标志位
                    flag |= O_NONBLOCK;//非阻塞
                    fcntl(cfd, F_SETFL, flag);//设置标志位
                    // 将cfd上树
                    ev.data.fd = cfd;
                    // 在这里加上边沿触发方式
                    ev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                }
                else if (ev_ret[i].events & EPOLLIN)
                {
                    // cfd的读事件变化
                    while (1)
                    {// 循环读
                        char buf[4] = ""; 
                        //注意如果非阻塞读，缓冲区无数据会返回-1，设置errno为EAGAIN
                        int n = read(ev_ret[i].data.fd, buf, sizeof(buf));
                        if (n < 0)
                        {
                            if(errno==EAGAIN) break;
                             //出错下树
                            perror("");
                            close(ev_ret[i].data.fd);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, ev_ret[i].data.fd, &ev_ret[i]);
                            break;
                        }
                        else if (n == 0)
                        {
                            cout << "client close" << endl;
                            close(ev_ret[i].data.fd);
                            // 下树
                            epoll_ctl(epfd, EPOLL_CTL_DEL, ev_ret[i].data.fd, &ev_ret[i]);
                            break;
                        }
                        else
                        {
                            // cout << buf << endl;
                            write(STDOUT_FILENO, buf, 4);
                            write(ev_ret[i].data.fd, buf, n);
                        }
                    }
                }
            }
        }
    }
    return 0;
}