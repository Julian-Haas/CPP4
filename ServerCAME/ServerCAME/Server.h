#pragma once
#pragma once
#include <WinSock2.h>
#include <iostream>
#include<vector>
#include <string>
#include <map>
#include<array>
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
	int sendedInts[2]; 
	float SendedPositions[3]; 
	enum protocol;
	char request[4096];
	std::map<int, Position> playerData; 
	float startPosOffset; 
	int playerCount; 
	int currentPlayerID; 
	int requestCode; 
	int answerCode; 
	int maxPlayerCount = 2; 
	//member functions: 
	void SendToClient(SOCKET i, const char* msg);
	void HandleIncomingRequest(bool& readingRequest, SOCKET i);
	void ReadMessage(const char* message);
	void RegisterNewPlayer();

	SOCKET listenerSocket; 

	void UpdatePlayerPosition();

	std::array<char, 20> PrepareMessage();


public:
	Server();
	void UnregisterPlayer();
	int InitServer(int argc, char* argv[]);

};
