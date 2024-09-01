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
	SendLogOut = 103
};

class Server
{
private:
	float receivedFloats[3]; // 3er array; mutmaßlich komplett sparen, datentyp nicht in namen
	char dataToSend[20]; // getrennt halten vom request
	char request[12]; // größe anpassen aufs maximum was reinkommen kann
	std::map<SOCKET, Position> playerData; // ok
	float currentPlayerID; // byte
	float playerCount; // byte
	float maxPlayerCount; // byte
	std::vector<SOCKET> sockets; // name nicht erklärend
	SOCKET currentPlayerSocket; // int und eventuell lokal
	SOCKET listenerSocket;
	SOCKET maxSocket; // möglicherweise int
	fd_set master; // ok; der master hat sichts zu sagen! allein der commander hat die macht!
	std::vector<float> playerNumbers; // byte
	bool isServerRunning;
	void PrintMap();
	void HandleNewConnection();
	void CheckForIncomingData();
	void HandleIncomingRequest(SOCKET i);
	void InitWinSockLibrary();
	void SendMessageToClient(SOCKET i, float answercode);
	void WSAError(std::string failedprocess);
	void InitListenerSocket();
	void InitNonBlockingMode(SOCKET socket);
public:
	ME_API ~Server() = default;
	ME_API void InitServer();
	ME_API Server();
};