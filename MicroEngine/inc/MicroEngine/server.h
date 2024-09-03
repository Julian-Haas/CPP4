#pragma once
#include "me_interface.h"
#include <WinSock2.h>

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
	{
	}
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
namespace me
{
	class Server
	{
	public:
		ME_API Server();
		ME_API ~Server() = default;
		ME_API void InitServer();
	private:
		float receivedFloats[3]; // 3er array; mutmaßlich komplett sparen, datentyp nicht in namen
		char dataToSend[20]; // getrennt halten vom request
		char request[12]; // größe anpassen aufs maximum was reinkommen kann
		float playerCount; // byte
		float maxPlayerCount; // byte
		std::vector<float> m_playerNumbers; // byte
		std::vector<SOCKET> m_ClientSockets;
		std::map<SOCKET, Position> playerData;
		SOCKET listenerSocket;
		int maxSocket;
		fd_set master;
		bool m_IsServerRunning;
		void PrintMap();
		void HandleNewConnection();
		void CheckForIncomingData();
		void HandleIncomingRequest(SOCKET i);
		void InitWinSockLibrary();
		void SendMessageToClient(SOCKET dataPlayerSocket, SOCKET targetPlayerSocket, float answerCode);
		void WSAError(std::string failedprocess);
		void InitListenerSocket();
		void InitNonBlockingMode(SOCKET socket);
		void HandleDisconnectedPlayer(SOCKET i);
	};
}