#include "stdafx.h"
#include "MEServer.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <fstream>
#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <chrono>
#include <optional>
#include <thread>
#include "me_interface.h"
//#pragma comment (lib, "ws2_32.lib")
//#pragma comment (lib, "iphlpapi.lib")


void Server::SendToClient(SOCKET i)
{
	int result = send(i, dataToSend, sizeof(dataToSend), 0);
	if (result == SOCKET_ERROR)
	{
		std::cout << "send failed(server)\n" << std::to_string(result) << "\n";
		std::cout << WSAGetLastError();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::RegisterNewPlayer()
{
	if (playerCount >= maxPlayerCount)
	{
		answerCode = (int)JoinAnswerFailed;
		return;
	}
	if (playerData.size() == 0)
	{
		currentPlayerID = 0;
	}
	else
	{
		//suche freie zahl
		auto it = playerData.begin();
		float newID = -1.0f;
		float i = 0.0f;
		while (it != playerData.end())
		{
			if (it->first != i)
			{
				newID = i;
				std::cout << "Set id at index: " << i << std::endl;
				break;
			}
			it++;
			i++;
		}
		if (newID == -1.0f)
		{
			auto lastKey = std::prev(playerData.end());
			newID = lastKey->first + 1.0f;
		}
		currentPlayerID = newID;
	}
	playerData.insert(std::make_pair(currentPlayerID, Position(currentPlayerSocket, 0, 0, 30)));
	playerCount++;
	answerCode = (int)JoinAnswerSucessful;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::InitServer()
{
	OpenDebugConsole();
	if (!InitListenerSocket()) {
		return false;
	}
	if (!InitClassParameters()) {
		return false;
	}
	isServerRunning = true;
	while (isServerRunning) {
		AcceptIncomingConnections();
		CheckForIncomingData();
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::HandleIncomingRequest(SOCKET i)
{
	memcpy(&recievedFloats, request, sizeof(recievedFloats));
	int msgCode = static_cast<int>(recievedFloats[0]);
	currentPlayerID = recievedFloats[1];
	switch (msgCode)
	{
	case JoinRequest:
		currentPlayerSocket = i;
		RegisterNewPlayer();
		PrepareMessage();
		SendToClient(i);
		for (const auto& pair : playerData) {
			SOCKET otherPlayerSocket = pair.second.playersocket;
			if (otherPlayerSocket == currentPlayerSocket) {
				answerCode = ProceedData;
				PrepareMessage();
				SendToClient(otherPlayerSocket);
			}
		}
		break;
	case SendPosition:
		for (const auto& pair : playerData) {
			SOCKET otherPlayerSocket = pair.second.playersocket;
			if (otherPlayerSocket == currentPlayerSocket) {
				answerCode = ProceedData;
				PrepareMessage();
				SendToClient(otherPlayerSocket);
			}
		}
		break;
	default:
		std::cout << "Unhandled request: " << msgCode << std::endl;
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::AcceptIncomingConnection()
{
	if (FD_ISSET(listenerSocket, &reads)) {
		SOCKET newSocket = accept(listenerSocket, nullptr, nullptr);
		if (newSocket == INVALID_SOCKET) {
			DisplayWSAError("accept");
			//assume graceful handling here was already done
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::CheckForIncomingData()
{
	const struct timeval LongTimeout = { 5, 0 };
	FD_ZERO(&reads);
	reads = master;
	int selectResult = select(static_cast<int>(maxSocket + 1), &reads, nullptr, nullptr, &LongTimeout);
	if (selectResult < 0) {
		DisplayWSAError("select");
		//assume graceful handling here was already done
		return;
	}

	//if listener -> accept
	for (SOCKET s : sockets) {
		FD_ISSET(s, &reads);


	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::InitListenerSocket()
{
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* bindAddress = nullptr;
	int bindResult = getaddrinfo("127.0.0.1", "8080", &hints, &bindAddress);
	if (bindResult != 0) {
		DisplayWSAError("getaddrinfo");
		WSACleanup();
		return false;
	}
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET) {
		DisplayWSAError("socket");
		freeaddrinfo(bindAddress);
		WSACleanup();
		return false;
	}
	if (bind(listenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen)) != 0) {
		DisplayWSAError("bind");
		closesocket(listenerSocket);
		freeaddrinfo(bindAddress);
		WSACleanup();
		return false;
	}
	int listenResult = listen(listenerSocket, 20);
	if (listenResult == SOCKET_ERROR) {
		DisplayWSAError("listen");
		closesocket(listenerSocket);
		freeaddrinfo(bindAddress);
		WSACleanup();
		return false;
	}
	if (!InitNonBlockingMode(listenerSocket)) {
		closesocket(listenerSocket);
		freeaddrinfo(bindAddress);
		WSACleanup();
		return false;
	}
	maxSocket = listenerSocket;
	FD_ZERO(&master);
	FD_SET(listenerSocket, &master);
	sockets.push_back(listenerSocket);
	freeaddrinfo(bindAddress);
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::InitNonBlockingMode(SOCKET socket)
{
	if (socket == INVALID_SOCKET) {
		std::cerr << "Failed to make Socket non-blocking. Invalid Socket" << std::endl;
		return false;
	}
	u_long mode = 1;
	int result = ioctlsocket(socket, FIONBIO, &mode);
	if (result != 0) {
		DisplayWSAError("ioctlsocket");
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::DisplayWSAError(std::string failedprocess)
{
	std::cerr << failedprocess << " failed. WSA-Error-Code: ";
	std::cerr << WSAGetLastError() << std::endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::InitClassParameters()
{
	if (!InitListenerSocket()) return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Server::InitWinSockLibrary() {
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		DisplayWSAError("WSAStartup");
		return false;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::PrintPlayerData()
{
	for (const auto& pair : playerData) {
		std::cout << "Player-ID: " << pair.first << ", Value: " << std::endl;
		std::cout << "Socket!:" << pair.second.playersocket << std::endl;
		std::cout << "X: " << pair.second.x << std::endl;
		std::cout << "Y: " << pair.second.y << std::endl;
		std::cout << "Z: " << pair.second.z << std::endl;
		std::cout << std::endl;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server()
	: startPosOffset(1000243.3F)
	, playerCount(0)
	, currentPlayerID(0)
	, requestCode(0)
	, answerCode(0)
	, maxPlayerCount(2)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::OpenDebugConsole()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
	freopen_s(&file, "CONIN$", "r", stdin);
	std::cout << "Debug-Konsole gestartet." << std::endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Server::PrepareMessage()
{
	float x[5];
	x[0] = static_cast<float>(answerCode);
	x[1] = currentPlayerID;
	x[2] = playerData[currentPlayerID].x;
	x[3] = playerData[currentPlayerID].y;
	x[4] = playerData[currentPlayerID].z++;
	memcpy(&dataToSend, x, sizeof(dataToSend));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////