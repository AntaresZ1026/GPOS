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
    SockServ = socket(AF_INET, SOCK_STREAM, 0); //���ɹ��򷵻�һ��sockfd���׽�����������
            //һ���Ǵ����ַ�Ͷ˿ڵġ�������Ϣ����ʾ���洢ʹ��
    /*���� sockaddr_in �ṹ������ز���*/
    serv_sockaddr.sin_family = AF_INET;
    serv.sockaddr.sin_port = htons(PORT); //��һ���޷��Ŷ�������ֵת��Ϊ�����ֽ��򣬼����ģʽ(big-endian) 
    // printf("%d\n",INADDR_ANY);
// INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ�������е�ַ�����������ַ����
//һ����˵���ڸ���ϵͳ�о������Ϊ0ֵ��
    serv.sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //���������޷��ų�������ת���������ֽ�˳��
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
    ///�ɹ����طǸ������֣���������-1
    conn = accept(sockSer, (struct sockaddr*)&client_addr, &length);
    //���accpet�ɹ�����ô�䷵��ֵ�����ں��Զ����ɵ�һ��ȫ���������������������ؿͻ���TCP���ӡ�
    // accpet֮��ͻ����µ��׽���conn
    if (conn < 0)
    {
        perror("connect");
        exit(1);
    }
    char buffer[1024];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer), 0); //��TCP���ӵ���һ�˽������ݡ�
        /*�ú����ĵ�һ������ָ�����ն��׽�����������
 �ڶ�������ָ��һ�����������û������������recv�������յ������ݣ�
 ����������ָ��buf�ĳ��ȣ�
 ���ĸ�����һ����0*/
        if (strcmp(buffer, "exit\n") == 0) //���û���յ�TCP��һ�˷���������������ѭ�������
        {
            break;
        }
        printf("%s", buffer); //������յ��������������
        //����Ҫ�з������ݣ� ��������һ������������
        send(conn, buffer, len, 0); //��TCP���ӵ���һ�˷������ݡ�
    }
    close(conn); //��Ϊaccpet�������ӳɹ��󻹻�����һ���µ��׽�����������������Ҳ��Ҫ�ر�
    close(sockSer);   //�ر�socket�׽���������
    return 0;
}


#endif // !__SERVER_H_