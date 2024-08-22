#pragma once
#include "me_interface.h"
#include <WinSock2.h>
#include "MEServerHelper.h"

struct Position
{
public:
	SOCKET playersocket;
	float x;
	float y;
	float z;
	Position() = default;
	Position(SOCKET socket, float xPos, float yPos, float zPos)
		: playersocket(socket)
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
	std::map<float, Position> playerData;
	float maxPlayerCount = 2;
	float currentPlayerID;
	float startPosOffset;
	float playerCount;
	float requestCode;
	float answerCode;
	std::vector<SOCKET> sockets;
	SOCKET currentPlayerSocket;
	SOCKET listenerSocket;
	SOCKET maxSocket;
	addrinfo hints;
	fd_set master;
	fd_set reads;
	WSADATA wsaData;
	bool isServerRunning;
	void AcceptIncomingConnection();
	void CheckForIncomingData();
	void HandleIncomingRequest(SOCKET i);
	void SendToClient(SOCKET i);
	void RegisterNewPlayer();
	void OpenDebugConsole();
	bool InitWinSockLibrary();
	void PrintPlayerData();
	void PrepareMessage();
	void DisplayWSAError(std::string failedprocess);
	bool InitClassParameters();
	bool InitListenerSocket();
	bool InitNonBlockingMode(SOCKET socket);
public:
	ME_API ~Server() = default;
	ME_API bool InitServer();
	ME_API Server();
};