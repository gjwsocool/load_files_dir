#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include "base.h"
using namespace std;
// 错误处理函数
static void error_handle(string opt,string message){
    perror(opt.c_str());
    cout << message << endl;
    exit(1);
}
int main(int argc, char *argv[]){//argc是参数个数
    //参数需要2个：运行文件+端口
    if(argc < 2){       
        cout << "less" << argv[0] << "port" << endl;
        exit(1);                        
    }
    //创建套接字
    int serve_sock = socket(PF_INET, SOCK_STREAM, 0);   
    if(serve_sock==-1){
        error_handle("socket", "socket() error");
    }
    //绑定地址
    struct sockaddr_in serve_addr;
    serve_addr.sin_family = AF_INET;
    //htonl---host to net long
    //将一个32位的主机字节序变成网络字节序
    serve_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serve_addr.sin_port = htons(atoi(argv[1]));
    //返回指针要传地址，而且要强制类型转换成sockaddr*类型
    int ret = bind(serve_sock, (struct sockaddr *)&serve_addr, sizeof(serve_addr));
    if(ret==-1){
        error_handle("bind", "bind() error");
    }
    //监听
    if(listen(serve_sock,5)==-1){
        error_handle("listen", "listen() error");  
    }
    //accept--接受客户端的请求
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size=sizeof(client_addr);
    //while(1)循环应答
    while(1){
        client_sock = accept(serve_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if(client_sock ==-1){
            continue;//接受新的请求
        }
        //接收新的客户端请求，进行客户端数据处理
        //inet_ntoa():将一个网络字节序的IP地址转化为点分十进制的IP地址（字符串）。
        cout << "Accept New Client : " << inet_ntoa(client_addr.sin_addr) << \
                           ", port : " << ntohs(client_addr.sin_port) << std::endl;
        cout << "Start Recv Client Data........." << endl;
        /*传输信息*/
        char message[BUFFSIZE];
        memset((void *)&message, 0, BUFFSIZE);//传指针
        int MessageLen;
        //读取客户端的请求数据
        while((MessageLen=read(client_sock,message,BUFFSIZE))!=0){
            cout << "Server recv from Client:" << message << endl;
            //将消息回传给客户端
            write(client_sock, message, MessageLen);
            //清空消息
            memset(message, 0, BUFFSIZE);
        }
        close(client_sock);
    }
    //服务器关闭
    close(serve_sock);
    return 0;
}
    