#pragma once
#include "me_interface.h"
#include <iostream>
#include<vector>
#include <string>
#include <map>
#include<array>
#include<Windows.h>
#include<fstream>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")
#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <chrono>
#include <optional>
#include "MEServerHelper.h"
#include <thread>
#include <array>
#include "me_interface.h"

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
	float SendedPositions[3];
	char dataToSend[20];
	char request[20];
	std::map<float, Position> playerData;
	float startPosOffset;
	float playerCount;
	float currentPlayerID;
	float requestCode;
	float answerCode;
	float maxPlayerCount = 2;
	SOCKET currentPlayerSocket;
	SOCKET listenerSocket;
	SOCKET maxSocket;
	std::vector<SOCKET> clientSockets;
	fd_set master;
	addrinfo hints;
	WSAData d;
	void SendToClient(SOCKET i);
	void RegisterNewPlayer();
	void OpenDebugConsole();
	void PrintPlayerData();
	void PrepareMessage();
	void HandleIncomingRequest(SOCKET i);
	void UpdateServer();
	void CloseClientSocket(SOCKET clientSocket);
public:
	ME_API Server();
	ME_API ~Server() = default;
	ME_API void UnregisterPlayer();
	ME_API int InitServer();
};