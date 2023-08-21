#ifndef _CLIENT_H_
#define _CLIENT_H_
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#define SERVERADDR "58.87.95.47"
#define CLIENTADDR "0.0.0.0"
#define PORT 21026
#define SEND_PATH "D:/GPOSPATH/Send"
#define RECV_PATH "D:/GPOSPATH/Send"

#pragma comment(lib,"ws2_32.lib")

namespace GPOS {
	class GPOS_Sock {
	private:
		std::string name;
		SOCKET self;
		SOCKADDR_IN SOCKADDR;
	public:
		//whereto = 1代表发往服务器， 0代表发往客户端
		GPOS_Sock(std::string sockname, bool whereto) {
			name = sockname;
			SOCKADDR.sin_family = AF_INET;
			SOCKADDR.sin_port = htons(PORT);
			if (whereto == true) SOCKADDR.sin_addr.S_un.S_addr = inet_addr(SERVERADDR);
			else SOCKADDR.sin_addr.S_un.S_addr = inet_addr(CLIENTADDR);
			Sock_init();
		}
		void Sock_init();
		void Sock_connect();
		void Sock_bind();
		void Sock_accept();
		void Sock_send();
		void Sock_recv();
		~GPOS_Sock() {
			closesocket(self);
		};

	};

	/*class FILE {
	private:
		std::fstream file_send;
		std::fstream file_recv;
	public:

	};*/
}
#endif