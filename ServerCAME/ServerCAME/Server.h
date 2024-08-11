#pragma once
#pragma once
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include <string>

struct Position
{
public:
	float x;
	float y;
	float z;

	Position(float xPos, float yPos, float zPos)
		: x(xPos)
		, y(yPos)
		, z(zPos)
	{}

	~Position()
	{}
};

class Server
{
private:
	char request[4096];

	//member functions: 
	void SendToClient(SOCKET i, std::string msg);
	void HandleIncomingRequest(bool& readingRequest, SOCKET i);
	void RegisterNewPlayer(Position pos);

	void UpdatePlayerPosition(int playerID, Position pos);

	void PrepareMessage(Position pos);
	void PrepareMessage();
	
	//protocol enum 
	enum protocol;

public:
	Server();
	int InitServer(int argc, char* argv[]);

};
