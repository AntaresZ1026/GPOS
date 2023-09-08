#ifndef __SERVER_H_
#define __SERVER_H_
#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 21026
class MSock {
private:
    int SockServ;
    struct sockaddr_in serv_sockaddr;
public:
	MSock();
};

MSock::MSock() {
    SockServ = socket(AF_INET, SOCK_STREAM, 0); //若成功则返回一个sockfd（套接字描述符）
            //一般是储存地址和端口的。用于信息的显示及存储使用
    /*设置 sockaddr_in 结构体中相关参数*/
    serv_sockaddr.sin_family = AF_INET;
    serv.sockaddr.sin_port = htons(PORT); //将一个无符号短整型数值转换为网络字节序，即大端模式(big-endian) 
    // printf("%d\n",INADDR_ANY);
// INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。
//一般来说，在各个系统中均定义成为0值。
    serv.sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //将主机的无符号长整形数转换成网络字节顺序。
    if (bind(SockServ, (struct sockaddr*)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(sockSer, QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    ///成功返回非负描述字，出错返回-1
    conn = accept(sockSer, (struct sockaddr*)&client_addr, &length);
    //如果accpet成功，那么其返回值是由内核自动生成的一个全新描述符，代表与所返回客户的TCP连接。
    // accpet之后就会用新的套接字conn
    if (conn < 0)
    {
        perror("connect");
        exit(1);
    }
    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0); //从TCP连接的另一端接收数据。
        /*该函数的第一个参数指定接收端套接字描述符；
 第二个参数指明一个缓冲区，该缓冲区用来存放recv函数接收到的数据；
 第三个参数指明buf的长度；
 第四个参数一般置0*/
        if (strcmp(buffer, "exit\n") == 0) //如果没有收到TCP另一端发来的数据则跳出循环不输出
        {
            break;
        }
        printf("%s", buffer); //如果有收到数据则输出数据
        //必须要有返回数据， 这样才算一个完整的请求
        send(conn, buffer, len, 0); //向TCP连接的另一端发送数据。
    }
    close(conn); //因为accpet函数连接成功后还会生成一个新的套接字描述符，结束后也需要关闭
    close(sockSer);   //关闭socket套接字描述符
    return 0;
}


#endif // !__SERVER_H_