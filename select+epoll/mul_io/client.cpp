#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

#define BUFFSIZE 1024

//出错调用函数
static void error_handle(std::string opt, std::string message)
{
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[]){
    //判断参数是否合法
    if(argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " <IP> <port>" << std::endl;
        exit(1);
    }
    //创建套接字
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        error_handle("socket", "socket() error.");
    }
    //绑定客户端
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    int ret = connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(ret < 0){
        error_handle("connect", "connect() error.");
    }
    std::cout << "Connect Success" << std::endl;
    //进入数据处理
    char message[BUFFSIZE];
    int strLen;
    while(1)
    {
        std::cout << "Please Input Message(Q to quit) : " << std::endl;
        std::cin >> message;
        if((!strcmp(message, "q")) || (!strcmp(message, "Q"))){
            //退出客户端
            break;
        }
        //将数据发送给服务端
        write(server_fd, message, strlen(message));
        //读取服务端回传的数据
        strLen = read(server_fd, message, BUFFSIZE-1);
        message[strLen] = 0;
        //打印输出
        std::cout << "Echo Message : " << message << std::endl;
    }
    //关闭套接字
    close(server_fd);
    return 0;
}