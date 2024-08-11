#pragma once
#pragma once
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include <string>
class Server
{
private:
	char request[4096];

	//member functions: 
	void SendToClient(SOCKET i, std::string msg);


	void HandleIncomingRequest(bool& readingRequest, SOCKET i);
	//protocol enum 
	enum protocol;

public:
	Server();
	int InitServer(int argc, char* argv[]);

};