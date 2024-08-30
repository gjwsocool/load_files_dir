#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include "base.h"
using namespace std;
// 出错调用函数
static void error_handle(std::string opt, std::string message)
{
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}
int main(int argc,char* argv[]){
    if(argc < 3){
        cout << "Input IP and Port!" << endl;
    }
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1){
        error_handle("socket", "socket() error");
    }
    //向服务器请求连接
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    int ret =connect(sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(ret==-1){
        error_handle("connect", "connect() error");
    }
    cout << "connect success!!" << endl;
    //数据处理
    char message[BUFFSIZE];
    while (1){
        cout << "Please Input Message(Q to quit) : " << endl;
        cin >> message;
        if((!strcmp(message, "q")) || (!strcmp(message, "Q"))){
            break;
        }
        //把数据发送给服务端
        write(sock,message,strlen(message));
        //读取u服务端回传的消息
        int len = read(sock, message, BUFFSIZE - 1);
        message[len] = 0;
        cout << "echo message:" << message << endl;
    }
    close(sock);
    return 0;
}