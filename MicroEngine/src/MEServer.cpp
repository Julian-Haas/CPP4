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
void Server::InitServer()
{
	Beep(750, 300);
	OpenDebugConsole();
	InitWinSockLibrary();
	InitListenerSocket();
	isServerRunning = true;
	while (isServerRunning) CheckForIncomingData();
}
void Server::HandleIncomingRequest(SOCKET i)
{
	int bytesReceived = recv(i, request, sizeof(request), 0);
	memcpy(&receivedFloats, request, sizeof(receivedFloats));
	int msgCode = static_cast<int>(receivedFloats[0]);
	if (msgCode != SendPosition) return;
	currentPlayerSocket = i;
	currentPlayerID = receivedFloats[1];
	float posX = receivedFloats[2];
	float posY = receivedFloats[3];
	float posZ = receivedFloats[4];
	playerData[i] = Position(currentPlayerID, posX, posY, posZ);
	for (const auto& pair : playerData) {
		SOCKET otherPlayerSocket = pair.first;
		if (otherPlayerSocket == currentPlayerSocket) { //needs to be !=
			SendMessageToClient(i, ProceedData);
		}
	}
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
	//system("cls");
	//for (SOCKET s : sockets) {
	//	std::cout << s << "\n";
	//}
	float newPlayerID = playerNumbers.front();
	playerNumbers.erase(playerNumbers.begin());
	playerData.insert(std::make_pair(newSocket, Position(newPlayerID, 0, 0, 30)));
	++playerCount;
	SendMessageToClient(newSocket, JoinRequestAccepted);
	//inform others
}
void Server::CheckForIncomingData()
{
	const struct timeval LongTimeout = { 1, 0 };
	FD_ZERO(&reads);
	reads = master;
	int selectResult = select(static_cast<int>(maxSocket + 1), &reads, nullptr, nullptr, &LongTimeout);
	if (selectResult < 0) WSAError("select");
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
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* bindAddress = nullptr;
	int bindResult = getaddrinfo("127.0.0.1", "8080", &hints, &bindAddress);
	if (bindResult != 0) {
		freeaddrinfo(bindAddress);
		WSAError("getaddrinfo");
	}
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET) {
		freeaddrinfo(bindAddress);
		WSAError("socket");
	}
	if (bind(listenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen)) != 0) {
		freeaddrinfo(bindAddress);
		WSAError("bind");
	}
	freeaddrinfo(bindAddress);
	int listenResult = listen(listenerSocket, 20);
	if (listenResult == SOCKET_ERROR) WSAError("listen");
	InitNonBlockingMode(listenerSocket);
	maxSocket = listenerSocket;
	FD_ZERO(&master);
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
	int result = ioctlsocket(socket, FIONBIO, &mode);
	if (result != 0) WSAError("ioctlsocket");
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
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) WSAError("WSAStartup");
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
}
void Server::OpenDebugConsole()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
	freopen_s(&file, "CONIN$", "r", stdin);
}
void Server::SendMessageToClient(SOCKET i, float answerCode)
{
	float x[5];
	x[0] = answerCode;
	x[1] = currentPlayerID;
	x[2] = playerData[static_cast<int>(i)].x;
	x[3] = playerData[static_cast<int>(i)].y;
	x[4] = playerData[static_cast<int>(i)].z;
	memcpy(&dataToSend, x, sizeof(dataToSend));
	int result = send(i, dataToSend, sizeof(dataToSend), 0);
	if (result == SOCKET_ERROR) WSAError("send");
}