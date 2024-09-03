#include "stdafx.h"
#include "MEServer.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include < conio.h >
#include "say.h"

void Server::InitServer()
{
	InitWinSockLibrary();
	InitListenerSocket();
	isServerRunning = true;
	while (isServerRunning) CheckForIncomingData();
}
void Server::HandleIncomingRequest(SOCKET i)
{
	int bytesReceived = recv(i, request, sizeof(request), 0);
	if (bytesReceived == -1) {
		HandleDisconnectedPlayer(i);
		return;
	}
	memcpy(&receivedFloats, request, sizeof(receivedFloats));
	currentPlayerSocket = i;
	//abschließen A
	float posX = receivedFloats[0];
	float posY = receivedFloats[1];
	float posZ = receivedFloats[2];
	//aufschließen A
	playerData[i] = Position(playerData[i].playerID, posX, posY, posZ);
	for (const auto& pair : playerData) {
		SOCKET otherPlayerSocket = pair.first;
		if (otherPlayerSocket != currentPlayerSocket) {
			SendMessageToClient(otherPlayerSocket, ProceedData);
		}
	}
}
void Server::HandleNewConnection()
{
	SOCKET newSocket = accept(listenerSocket, nullptr, nullptr);
	if (newSocket == INVALID_SOCKET) WSAError("accept");
	if (playerCount >= maxPlayerCount)
	{
		SendMessageToClient(newSocket, JoinRequestDenied);
		closesocket(newSocket);
		return;
	}
	FD_SET(newSocket, &master);
	sockets.push_back(newSocket);
	float newPlayerID = playerNumbers.front();
	playerNumbers.erase(playerNumbers.begin());
	playerData.insert(std::make_pair(newSocket, Position(newPlayerID, 0, 0, 30)));
	++playerCount;
	currentPlayerSocket = newSocket;
	currentPlayerID = playerData[newSocket].playerID;
	SendMessageToClient(newSocket, JoinRequestAccepted);
}
void Server::CheckForIncomingData()
{
	const struct timeval LongTimeout = { 1, 0 };
	fd_set reads;
	reads = master;
	int result_select = select(static_cast<int>(maxSocket + 1), &reads, nullptr, nullptr, &LongTimeout);
	if (result_select < 0) WSAError("select");
	for (SOCKET s : sockets) {
		if (!FD_ISSET(s, &reads)) continue;
		if (s == listenerSocket) {
			HandleNewConnection();
		}

		else {
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
	if (result_getaddrinfo != 0) {
		freeaddrinfo(bindAddress);
		WSAError("getaddrinfo");
	}
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET) {
		freeaddrinfo(bindAddress);
		WSAError("socket");
	}
	int result_bind = bind(listenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen));
	if (result_bind != 0) {
		freeaddrinfo(bindAddress);
		WSAError("bind");
	}
	freeaddrinfo(bindAddress);
	int result_listen = listen(listenerSocket, 20);
	if (result_listen == SOCKET_ERROR) WSAError("listen");
	InitNonBlockingMode(listenerSocket);
	maxSocket = listenerSocket;
	FD_SET(listenerSocket, &master);
	sockets.push_back(listenerSocket);
}
void Server::InitNonBlockingMode(SOCKET socket)
{
	if (socket == INVALID_SOCKET) {
		std::cerr << "Failed to make Socket non-blocking. Invalid Socket" << std::endl;
		return;
	}
	u_long mode = 1;
	int result_ioctlsocket = ioctlsocket(socket, FIONBIO, &mode);
	if (result_ioctlsocket != 0) WSAError("ioctlsocket");
}
void Server::HandleDisconnectedPlayer(SOCKET i)
{
	auto it = playerData.find(i);
	if (it == playerData.end()) {
		std::cerr << "Error: Attempted to handle a disconnected player that is not in playerData." << std::endl;
	}
	playerNumbers.push_back(it->second.playerID);
	auto vecIt = std::remove(sockets.begin(), sockets.end(), i);
	if (vecIt != sockets.end()) {
		sockets.erase(vecIt, sockets.end());
	}
	else {
		std::cerr << "Warning: Socket not found in sockets vector." << std::endl;
	}
	FD_CLR(i, &master);
	playerData.erase(i);
	if (closesocket(i) == SOCKET_ERROR) WSAError("closesocket");
	Say("jsdzgfbusdzfgsduzfgsduzfgsdfzugsdf");
}
void Server::WSAError(std::string failedprocess)
{
	std::cerr << failedprocess << " failed. WSA-Error-Code: ";
	std::cerr << WSAGetLastError() << std::endl;
	if (listenerSocket != INVALID_SOCKET) {
		closesocket(listenerSocket);
		listenerSocket = INVALID_SOCKET;
	}
	WSACleanup();
	std::cerr << "This Error is critical, the apllication will close. Please press any key.";
	_getch();
	exit(1);
}
void Server::InitWinSockLibrary() {
	WSADATA wsaData;
	int result_WSAStartup = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result_WSAStartup != 0) WSAError("WSAStartup");
}
Server::Server()
	: playerCount(0)
	, currentPlayerID(0)
	, maxPlayerCount(8)
{
	playerNumbers.reserve(static_cast<int>(maxPlayerCount));
	for (float i = 0.0f; i < maxPlayerCount; ++i) {
		playerNumbers.push_back(i);
	}
	playerData.clear();
	FD_ZERO(&master);
}
void Server::SendMessageToClient(SOCKET i, float answerCode)
{
	currentPlayerID = playerData[static_cast<int>(currentPlayerSocket)].playerID;
	float x[5];
	//abschließen A
	x[0] = answerCode;
	x[1] = currentPlayerID;
	x[2] = playerData[static_cast<int>(currentPlayerSocket)].x;
	x[3] = playerData[static_cast<int>(currentPlayerSocket)].y;
	x[4] = playerData[static_cast<int>(currentPlayerSocket)].z;
	//aufschließen A
	memcpy(&dataToSend, x, sizeof(dataToSend));
	int result_send = send(i, dataToSend, sizeof(dataToSend), 0);
	if (result_send == SOCKET_ERROR) WSAError("send");
}
void Server::PrintMap()
{
	for (const auto& pair : playerData) {
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