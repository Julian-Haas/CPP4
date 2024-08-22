#pragma once
#include "me_interface.h"
#include <WinSock2.h>
#include "MEServerHelper.h"

struct Position
{
public:
	float playerID;
	float x;
	float y;
	float z;
	Position() = default;
	Position(float sentPlayerID, float xPos, float yPos, float zPos)
		: playerID(sentPlayerID)
		, x(xPos)
		, y(yPos)
		, z(zPos)
	{}
	~Position() = default;
};

enum protocol
{
	JoinAnswerSucessful = 1,
	JoinAnswerFailed = 2,
	ProceedData = 3,
	JoinRequest = 101,
	SendPosition = 102,
};

class Server
{
private:
	float recievedFloats[5];
	float SentPositions[3];
	char dataToSend[20];
	char request[20];
	std::map<SOCKET, Position> playerData;
	float maxPlayerCount = 2;
	float currentPlayerID;
	float playerCount;
	float requestCode;
	std::vector<SOCKET> sockets;
	SOCKET currentPlayerSocket;
	SOCKET listenerSocket;
	SOCKET maxSocket;
	addrinfo hints;
	fd_set master;
	fd_set reads;
	WSADATA wsaData;
	std::vector<float> playerNumbers;
	bool isServerRunning;
	void AcceptIncomingConnection();
	void CheckForIncomingData();
	void HandleIncomingRequest(SOCKET i);
	void RegisterNewPlayer(SOCKET i);
	void OpenDebugConsole();
	bool InitWinSockLibrary();
	void PrintPlayerData();
	void SendMessage(SOCKET i, float answercode);
	void DisplayWSAError(std::string failedprocess);
	bool InitListenerSocket();
	bool InitNonBlockingMode(SOCKET socket);
public:
	ME_API ~Server() = default;
	ME_API bool InitServer();
	ME_API Server();
};