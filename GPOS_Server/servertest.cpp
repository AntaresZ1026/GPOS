// server.cpp
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>

#define IMGMAXBUF 1024 * 512
#define PORT 21026
#define DEBUG
#define QUEUEMAXLEN 12

#ifdef DEBUG
#define DEBUG_P(x) std::cout << x << "\n"
#else
#define DEBUG_P(x)
#endif

/*
	MyException继承exception, 用来自定义异常处理
*/
class MyException : public std::exception {
public:
	MyException(const char* msg) : m_msg(msg) {

	}

	virtual const char* what() noexcept {
		return m_msg.c_str();
	}
private:
	std::string m_msg;
};

/*
	MySocket封装了socket操作，注意对m_addr.sin_addr.s_addr的赋值为INADDR_ANY
	使用时只需要声明MySocket，即可自动初始化，初始化失败返回ERR[xxx]
*/
class MySocket {
public:
	MySocket() { sock_init(); }
	~MySocket() {
		close(m_socket);
		close(soc_socket);
	}
	int GetSourceSocket() {
		return soc_socket;
	}
private:
	sockaddr_in m_addr, soc_addr;
	socklen_t m_addrlen = sizeof(m_addr);
	socklen_t soc_addrlen = sizeof(soc_addr);
	int m_socket, soc_socket;
	void sock_init();
};

void MySocket::sock_init() {
	try {
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socket == -1) {
			DEBUG_P("Failed to create socket.");
			throw MyException("ERR[000]");
		}
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons(PORT); // 请将端口号替换为您需要的端口
		m_addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(m_socket, (struct sockaddr*)&m_addr, sizeof(m_addr)) == -1) {
			DEBUG_P("Failed to bind socket.");
			throw MyException("ERR[001]");
		}

		if (listen(m_socket, QUEUEMAXLEN) == -1) {
			DEBUG_P("Failed to listen socket.");
			throw MyException("ERR[002]");
		}

		std::cout << "Socket init success.Server is listening now.\n";
		while (true) {
			soc_socket = accept(m_socket, (struct sockaddr*)&soc_addr, &soc_addrlen);
			if (soc_socket == -1) {
				DEBUG_P("Failed to accept.");
				throw MyException("ERR[004]");
			}
			std::cout << "Socket connection success.\n";
		}
		return;
	}
	catch (MyException& err) {
		std::cerr << err.what() << "\n";
		MySocket::~MySocket();
	}
}

/*
	IMGProcess用来处理图像操作
	图片的保存路径为/home/antaresz/GPOS/input(output)/usrID/bgr(src、com)
*/
class IMGProcess {
public:
	IMGProcess(std::string usrid, int IMGMaxSize, int recv_socket) {
		maxsize = IMGMaxSize;
		usrID = usrid;
		client_socket = recv_socket;
		SetDir(usrID);
	}

	void Process();

private:
	int maxsize;
	int tru_size;
	std::string senddir;
	std::string bgrrecvdir;
	std::string srcrecvdir;
	int client_socket;
	std::string usrID;
	char IMGSizeBuf[32];

	void SendIMG();
	void RecvIMG(bool mode);
	void SendIMGSize();
	void RecvIMGSize();
	void SetDir(std::string usr);
};

void IMGProcess::Process() {
	RecvIMGSize();
	RecvIMG(true);
	RecvIMG(false);
	SendIMGSize();
	SendIMG();
}

void IMGProcess::SetDir(std::string usr) {
	senddir = "/home/antaresz/GPOS/output/";
	senddir.append(usr);
	senddir.append("/com/");
	bgrrecvdir = "/home/antaresz/GPOS/input/";
	bgrrecvdir.append(usr);
	srcrecvdir = bgrrecvdir;
	bgrrecvdir.append("/bgr/");
	srcrecvdir.append("/src/");
}

void IMGProcess::RecvIMGSize() {
	recv(client_socket, IMGSizeBuf, sizeof(IMGSizeBuf), 0);
	tru_size = atoi(IMGSizeBuf);
}

void IMGProcess::SendIMGSize() {

	std::string sendname;

	sendname = senddir;
	sendname.append("000.png");

	std::ifstream imagefile(sendname, std::ios::binary | std::ios::ate);
	try {
		if (!imagefile.is_open()) {
			DEBUG_P("Failed to open the img to send.\n");
			imagefile.close();
			throw(MyException("ERR[005]"));
		}
	}
	catch (MyException& err) {
		std::cout << err.what() << "\n";
	}

	// 获取图片大小
	std::streamsize imagesize = imagefile.tellg();
	imagefile.seekg(0, std::ios::beg);

	// 发送图片大小
	snprintf(IMGSizeBuf, tru_size, "%d", imagesize);
	send(client_socket, IMGSizeBuf, strlen(IMGSizeBuf), 0);
}


void IMGProcess::SendIMG() {
	// 读取图片文件
	//缺少数目

	std::string sendname;

	sendname = senddir;
	sendname.append("000.png");

	std::ifstream imagefile(sendname, std::ios::binary | std::ios::ate);

	try {
		if (!imagefile.is_open()) {
			DEBUG_P("Failed to open the img to send.\n");
			imagefile.close();
			throw(MyException("ERR[005]"));
		}
	}
	catch (MyException& err) {
		std::cout << err.what() << "\n";
	}
	std::streamsize imagesize = imagefile.tellg();
	imagefile.seekg(0, std::ios::beg);
	// 发送图片数据
	char* imagedata = new char[tru_size];

	imagefile.read(imagedata, imagesize);

	send(client_socket, imagedata, imagesize, 0);

	delete[] imagedata;
	imagefile.close();

}

void IMGProcess::RecvIMG(bool mode) {
	//缺个大小判断

	char* recvdata = new char[IMGMAXBUF];
	int recvbytes = 0;
	int bytesread = 0;
	std::string recvname;

	if (mode == true) {
		recvname = srcrecvdir;
		recvname.append("000.png");
		//缺少数目
	}
	else {
		recvname = bgrrecvdir;
		recvname.append("000.png");
		//缺少数目
	}

	std::ofstream imagefile(recvname, std::ios::binary);

	while (recvbytes < tru_size) {
		bytesread = recv(client_socket, recvdata + recvbytes, IMGMAXBUF - recvbytes, 0);
		if (bytesread == -1 || bytesread == 0) {
			DEBUG_P("RecvIMG() end.\n");
			break;
		}
		recvbytes += bytesread;
	}


	imagefile.write(recvdata, tru_size);
	imagefile.close();

	delete[] recvdata;
	tru_size = 0;
	memset(IMGSizeBuf, 0, sizeof(IMGSizeBuf));
}

int main() {
	MySocket msock;
	MySocket* msock_p = &msock;
	IMGProcess imgprocess("antaresz", IMGMAXBUF, msock_p->GetSourceSocket());

	return 0;
}
