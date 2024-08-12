#pragma once
#pragma once
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include <string>
#include <map>
struct Position
{
public:
	float x;
	float y;
	float z;
	Position() {}
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
	//protocol enum 
	enum protocol;
	char request[4096];
	std::map<int, Position> playerData; 
	float startPosOffset; 
	int playerCount; 
	int currentPlayerID; 
	//member functions: 
	void SendToClient(SOCKET i, std::string msg);
	void HandleIncomingRequest(bool& readingRequest, SOCKET i);
	void ReadMessage(char* message);
	void RegisterNewPlayer(SOCKET i);
	void RegisterNewPlayer();

	void UpdatePlayerPosition(SOCKET i);

	std::string PrepareMessage(protocol msgCode);
	


public:
	Server();
	int InitServer(int argc, char* argv[]);

};
