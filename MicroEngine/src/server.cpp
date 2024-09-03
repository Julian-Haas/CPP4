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

namespace me {
	void Server::InitServer()
	{
		InitWinSockLibrary();
		InitListenerSocket();
		m_IsServerRunning = true;
		while (m_IsServerRunning) CheckForIncomingData();
	}
	void Server::HandleIncomingRequest(SOCKET currentPlayerSocket)
	{
		int bytesReceived = recv(currentPlayerSocket, request, sizeof(request), 0);
		if (bytesReceived == -1)
		{
			HandleDisconnectedPlayer(currentPlayerSocket);
			return;
		}
		memcpy(&receivedFloats, request, sizeof(receivedFloats));
		float posX = receivedFloats[0];
		float posY = receivedFloats[1];
		float posZ = receivedFloats[2];
		playerData[currentPlayerSocket] = Position(playerData[currentPlayerSocket].playerID, posX, posY, posZ);
		for (const auto& pair : playerData)
		{
			SOCKET otherPlayerSocket = pair.first;
			if (otherPlayerSocket != currentPlayerSocket)
			{
				SendMessageToClient(otherPlayerSocket, currentPlayerSocket, ProceedData);
			}
		}
	}
	void Server::HandleNewConnection()
	{
		SOCKET newSocket = accept(listenerSocket, nullptr, nullptr);
		if (newSocket == INVALID_SOCKET) WSAError("accept");
		if (playerCount >= maxPlayerCount)
		{
			SendMessageToClient(newSocket, newSocket, JoinRequestDenied);
			closesocket(newSocket);
			return;
		}
		FD_SET(newSocket, &master);
		m_ClientSockets.push_back(newSocket);
		float newPlayerID = m_playerNumbers.front();
		m_playerNumbers.erase(m_playerNumbers.begin());
		playerData.insert(std::make_pair(newSocket, Position(newPlayerID, 0, 0, 30)));
		++playerCount;
		SendMessageToClient(newSocket, newSocket, JoinRequestAccepted);
	}
	void Server::CheckForIncomingData()
	{
		const struct timeval LongTimeout = { 1, 0 };
		fd_set reads;
		reads = master;
		int result_select = select(maxSocket + 1, &reads, nullptr, nullptr, &LongTimeout);
		if (result_select < 0) WSAError("select");
		for (SOCKET s : m_ClientSockets) {
			if (!FD_ISSET(s, &reads)) continue;
			if (s == listenerSocket)
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
		listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
		if (listenerSocket == INVALID_SOCKET)
		{
			freeaddrinfo(bindAddress);
			WSAError("socket");
		}
		int result_bind = bind(listenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen));
		if (result_bind != 0)
		{
			freeaddrinfo(bindAddress);
			WSAError("bind");
		}
		freeaddrinfo(bindAddress);
		int result_listen = listen(listenerSocket, 20);
		if (result_listen == SOCKET_ERROR) WSAError("listen");
		InitNonBlockingMode(listenerSocket);
		maxSocket = static_cast<int>(listenerSocket);
		FD_SET(listenerSocket, &master);
		m_ClientSockets.push_back(listenerSocket);
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
		auto it = playerData.find(i);
		if (it == playerData.end())
		{
			std::cerr << "Error: Attempted to handle a disconnected player that is not in playerData." << std::endl;
		}
		m_playerNumbers.push_back(it->second.playerID);
		auto vecIt = std::remove(m_ClientSockets.begin(), m_ClientSockets.end(), i);
		if (vecIt != m_ClientSockets.end())
		{
			m_ClientSockets.erase(vecIt, m_ClientSockets.end());
		}
		else
		{
			std::cerr << "Warning: Socket not found in sockets vector." << std::endl;
		}
		FD_CLR(i, &master);
		playerData.erase(i);
		if (closesocket(i) == SOCKET_ERROR) WSAError("closesocket");
	}
	void Server::WSAError(std::string failedprocess)
	{
		std::cerr << failedprocess << " failed. WSA-Error-Code: ";
		std::cerr << WSAGetLastError() << std::endl;
		if (listenerSocket != INVALID_SOCKET)
		{
			closesocket(listenerSocket);
			listenerSocket = INVALID_SOCKET;
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
		: playerCount(0)
		, maxPlayerCount(8)
		, m_IsServerRunning(false)
	{
		m_playerNumbers.reserve(static_cast<int>(maxPlayerCount));
		for (float i = 0.0f; i < maxPlayerCount; ++i)
		{
			m_playerNumbers.push_back(i);
		}
		playerData.clear();
		FD_ZERO(&master);
	}
	void Server::SendMessageToClient(SOCKET dataPlayerSocket, SOCKET targetPlayerSocket, float answerCode)
	{
		int dataPlayerID = static_cast<int>(playerData[static_cast<int>(dataPlayerSocket)].playerID);
		float x[5];
		x[0] = answerCode;
		x[1] = (float)dataPlayerID;
		x[2] = playerData[static_cast<int>(dataPlayerSocket)].x;
		x[3] = playerData[static_cast<int>(dataPlayerSocket)].y;
		x[4] = playerData[static_cast<int>(dataPlayerSocket)].z;
		memcpy(&dataToSend, x, sizeof(dataToSend));
		int result_send = send(targetPlayerSocket, dataToSend, sizeof(dataToSend), 0);
		if (result_send == SOCKET_ERROR) WSAError("send");
	}
	void Server::PrintMap()
	{
		for (const auto& pair : playerData)
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