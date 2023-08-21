#include "Client.h"
#include <iostream>

namespace GPOS {
	void GPOS_Sock::Sock_init() {
		int temp;
		temp = socket(AF_INET, SOCK_STREAM, 0);
		if (temp != -1) std::cout << "Sock init success, id:" << self << std::endl;
		else {
			perror("Sock init fail");
			std::cout << errno << std::endl;
			exit(1);
		}
	}

	void GPOS_Sock::Sock_bind() {

	}
}