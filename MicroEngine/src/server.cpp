#include "stdafx.h"
#include "server.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include "say.h"


#include <bitset>

namespace me
{
	void Server::InitServer()
	{
		InitWinSockLibrary();
		InitListenerSocket();
		m_IsServerRunning = true;
		while (m_IsServerRunning) CheckForIncomingData();
	}
	void Server::HandleIncomingRequest(SOCKET currentPlayerSocket)
	{
		char dataToProcess[1024];
		int bytesToProcess = recv(currentPlayerSocket, dataToProcess, sizeof(dataToProcess), 0);
		if (bytesToProcess == -1)
		{
			HandleDisconnectedPlayer(currentPlayerSocket);
			return;
		}
		int positionToRead = 0;
		while (bytesToProcess > positionToRead)
		{
			uint8_t protocolCode = dataToProcess[positionToRead++];

			switch (protocolCode) {
			case SendPosition:
				if (bytesToProcess >= (positionToRead + sizeof(float) * 3))
				{
					float posX = *reinterpret_cast<float*>(&dataToProcess[positionToRead]);
					positionToRead += sizeof(float);
					float posY = *reinterpret_cast<float*>(&dataToProcess[positionToRead]);
					positionToRead += sizeof(float);
					float posZ = *reinterpret_cast<float*>(&dataToProcess[positionToRead]);
					positionToRead += sizeof(float);
					m_PlayerData[currentPlayerSocket] = Position(m_PlayerData[currentPlayerSocket].playerID, posX, posY, posZ);
					for (const auto& pair : m_PlayerData)
					{
						SOCKET otherPlayerSocket = pair.first;
						if (otherPlayerSocket != currentPlayerSocket)
						{
							SendMessageToClient(otherPlayerSocket, currentPlayerSocket, ProceedData);
						}
					}
				}
				break;
			case JoinRequest:
				//ignore this. will be implemented later
				break;
			case SendLogOut:
				//ignore this. will be implemented later
				break;
			default:
				break;
			}
		}
	}
	void Server::HandleNewConnection()
	{
		SOCKET newSocket = accept(m_ListenerSocket, nullptr, nullptr);
		if (newSocket == INVALID_SOCKET) WSAError("accept");
		if (m_PlayerCount >= m_MaxPlayerCount)
		{
			SendMessageToClient(newSocket, newSocket, JoinRequestDenied);
			closesocket(newSocket);
			return;
		}
		FD_SET(newSocket, &m_Master);
		m_ClientSockets.push_back(newSocket);
		float newPlayerID = m_playerNumbers.front();
		m_playerNumbers.erase(m_playerNumbers.begin());
		m_PlayerData.insert(std::make_pair(newSocket, Position(newPlayerID, 0, 0, 30)));
		++m_PlayerCount;
		SendMessageToClient(newSocket, newSocket, JoinRequestAccepted);
	}
	void Server::CheckForIncomingData()
	{
		const struct timeval LongTimeout = { 1, 0 };
		fd_set reads;
		reads = m_Master;
		int result_select = select(m_MaxSocket + 1, &reads, nullptr, nullptr, &LongTimeout);
		if (result_select < 0) WSAError("select");
		for (SOCKET s : m_ClientSockets)
		{
			if (!FD_ISSET(s, &reads)) continue;
			if (s == m_ListenerSocket)
			{
				HandleNewConnection();
			}
			else
			{
				HandleIncomingRequest(s);
			}
		}
	}
	void Server::InitListenerSocket()
	{
		const char* IP = "127.0.0.1";
		const char* Port = "8080";
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		addrinfo* bindAddress = nullptr;
		int result_getaddrinfo = getaddrinfo(IP, Port, &hints, &bindAddress);
		if (result_getaddrinfo != 0)
		{
			freeaddrinfo(bindAddress);
			WSAError("getaddrinfo");
		}
		m_ListenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
		if (m_ListenerSocket == INVALID_SOCKET)
		{
			freeaddrinfo(bindAddress);
			WSAError("socket");
		}
		int result_bind = bind(m_ListenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen));
		if (result_bind != 0)
		{
			freeaddrinfo(bindAddress);
			WSAError("bind");
		}
		freeaddrinfo(bindAddress);
		int result_listen = listen(m_ListenerSocket, 20);
		if (result_listen == SOCKET_ERROR) WSAError("listen");
		InitNonBlockingMode(m_ListenerSocket);
		m_MaxSocket = static_cast<int>(m_ListenerSocket);
		FD_SET(m_ListenerSocket, &m_Master);
		m_ClientSockets.push_back(m_ListenerSocket);
	}
	void Server::InitNonBlockingMode(SOCKET socket)
	{
		if (socket == INVALID_SOCKET)
		{
			std::cerr << "Failed to make Socket non-blocking. Invalid Socket" << std::endl;
			return;
		}
		u_long mode = 1;
		int result_ioctlsocket = ioctlsocket(socket, FIONBIO, &mode);
		if (result_ioctlsocket != 0) WSAError("ioctlsocket");
	}
	void Server::HandleDisconnectedPlayer(SOCKET i) {
		auto it = m_PlayerData.find(i);
		if (it == m_PlayerData.end())
		{
			std::cerr << "Error: Attempted to handle a disconnected player that is not in playerData." << std::endl;
		}
		m_playerNumbers.push_back(static_cast<uint8_t>(it->second.playerID));
		auto vecIt = std::remove(m_ClientSockets.begin(), m_ClientSockets.end(), i);
		if (vecIt != m_ClientSockets.end())
		{
			m_ClientSockets.erase(vecIt, m_ClientSockets.end());
		}
		else
		{
			std::cerr << "Warning: Socket not found in sockets vector." << std::endl;
		}
		FD_CLR(i, &m_Master);
		m_PlayerData.erase(i);
		if (closesocket(i) == SOCKET_ERROR) WSAError("closesocket");
	}
	void Server::WSAError(std::string failedprocess)
	{
		std::cerr << failedprocess << " failed. WSA-Error-Code: ";
		std::cerr << WSAGetLastError() << std::endl;
		if (m_ListenerSocket != INVALID_SOCKET)
		{
			closesocket(m_ListenerSocket);
			m_ListenerSocket = INVALID_SOCKET;
		}
		WSACleanup();
		std::cerr << "This Error is critical, the application will close. Please press any key.";
		_getch();
		exit(1);
	}
	void Server::InitWinSockLibrary()
	{
		WSADATA wsaData;
		int result_WSAStartup = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result_WSAStartup != 0) WSAError("WSAStartup");
	}
	Server::Server()
		: m_PlayerCount(0)
		, m_MaxPlayerCount(8)
		, m_IsServerRunning(false)
		, m_ListenerSocket(INVALID_SOCKET)
		, m_MaxSocket(0)
	{
		m_playerNumbers.reserve(m_MaxPlayerCount);
		for (uint8_t i = 0; i < m_MaxPlayerCount; i++)
		{
			m_playerNumbers.push_back(static_cast<uint8_t>(i));
		}
		m_PlayerData.clear();
		FD_ZERO(&m_Master);
	}
	void Server::SendMessageToClient(SOCKET dataPlayerSocket, SOCKET targetPlayerSocket, float answerCode)
	{
		int dataPlayerID = static_cast<int>(m_PlayerData[static_cast<int>(dataPlayerSocket)].playerID);
		float x[5];
		x[0] = answerCode;
		x[1] = (float)dataPlayerID;
		x[2] = m_PlayerData[static_cast<int>(dataPlayerSocket)].x;
		x[3] = m_PlayerData[static_cast<int>(dataPlayerSocket)].y;
		x[4] = m_PlayerData[static_cast<int>(dataPlayerSocket)].z;
		memcpy(&dataToSend, x, sizeof(dataToSend));
		int result_send = send(targetPlayerSocket, dataToSend, sizeof(dataToSend), 0);
		if (result_send == SOCKET_ERROR) WSAError("send");
	}
	void Server::PrintMap()
	{
		for (const auto& pair : m_PlayerData)
		{
			SOCKET socket = pair.first;
			const Position& position = pair.second;
			std::cout << "Socket: " << socket << std::endl;
			std::cout << "Player ID: " << position.playerID << std::endl;
			std::cout << "X: " << position.x << std::endl;
			std::cout << "Y: " << position.y << std::endl;
			std::cout << "Z: " << position.z << std::endl;
			std::cout << std::endl;
		}
	}
}