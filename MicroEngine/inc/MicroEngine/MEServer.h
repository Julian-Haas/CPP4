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
	JoinRequestAccepted = 1,
	JoinRequestDenied = 2,
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
	std::vector<SOCKET> sockets;
	SOCKET currentPlayerSocket;
	SOCKET listenerSocket;
	SOCKET maxSocket;
	fd_set master;
	fd_set reads;
	std::vector<float> playerNumbers;
	bool isServerRunning;
	void HandleNewConnection();
	void CheckForIncomingData();
	void HandleIncomingRequest(SOCKET i);
	void OpenDebugConsole();
	void InitWinSockLibrary();
	void PrintPlayerData();
	void SendMessageToClient(SOCKET i, float answercode);
	void WSAError(std::string failedprocess);
	void InitListenerSocket();
	void InitNonBlockingMode(SOCKET socket);
public:
	ME_API ~Server() = default;
	ME_API void InitServer();
	ME_API Server();
};