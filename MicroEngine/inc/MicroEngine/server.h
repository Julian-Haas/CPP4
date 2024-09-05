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
		ME_API ~Server();
		ME_API void InitServer();
	private:
		char m_DataToSend[20];
		uint8_t m_PlayerCount;
		uint8_t m_MaxPlayerCount;
		std::vector<uint8_t> m_PlayerNumbers;
		std::vector<SOCKET> m_ClientSockets;
		std::map<SOCKET, Position> m_PlayerData;
		SOCKET m_ListenerSocket;
		fd_set m_Master;
		int m_MaxSocket;
		bool m_IsServerRunning;
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