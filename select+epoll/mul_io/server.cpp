#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
using namespace std;

#define BUFFSIZE 1024

//出错调用函数
static void error_handle(std::string opt, std::string message){
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[]){
    //判断命令行参数合法性
    if(argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " <port>" << std::endl;
        exit(1);
    }

    //创建socket套接字
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        error_handle("socket", "socket() error.");
    }

    //绑定
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    int re = bind(server_fd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    if(re < 0){
        error_handle("bind", "bind() error.");
    }

    //监听TCP端口号
    re = listen(server_fd, 10);
    if(re < 0){
        error_handle("listen", "listen() error.");
    }
    int maxfd = server_fd;//最大的文件描述符
    fd_set oldset, rset;
    //加入前先清空
    FD_ZERO(&oldset);
    FD_ZERO(&rset);
    //将lfd加入oldset中
    FD_SET(server_fd, &oldset);
    while(1){
        //监听前内核拷贝原有数据
        rset = oldset;
        int n=select(maxfd + 1, &rset, NULL, NULL, NULL);
        if(n<0){
            perror("");
            break;
        }else if(n==0){
            continue;//重新监听
        }else{//n>0：文件描述符变化
            if(FD_ISSET(server_fd,&rset)){
                sockaddr_in client_addr;
                socklen_t client_size= sizeof(client_addr);
                char ip[16] = "";
                int client_fd=accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
                cout << "new client IP:" << inet_ntop(AF_INET, &client_addr.sin_addr, ip, 16);
                cout << ",port:" << ntohs(client_addr.sin_port) << endl;
                //将client_fd放到oldset
                FD_SET(client_fd, &oldset);
                if(client_fd>maxfd)
                    maxfd = client_fd;
            
                //如果只有client_fd变化，continue
                if(--n==0){
                    continue;
                }
            }//if
            //client_fd变化，遍历文件描述符集合，查找变化的client_fd
            for (int i = server_fd + 1; i <= maxfd;i++){
                if(FD_ISSET(i,&rset)){
                    char buf[BUFFSIZE] = "";
                    int ret = read(i, buf, sizeof(buf));
                    if(ret<0){
                        perror("");
                        //出错关闭client_fd,从oldset删除
                        close(i);
                        FD_CLR(i, &oldset);
                        continue;
                    }else if(ret==0){
                        cout << "client close" << endl;
                        close(i);
                        FD_CLR(i, &oldset);
                    }else{
                        cout << buf << endl;
                        write(i, buf, ret);
                    }
                }
            }
        } 
    }
    return 0;
}