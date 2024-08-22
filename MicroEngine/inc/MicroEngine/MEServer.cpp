#include "stdafx.h"
#include "MEServer.h"
void Server::SendToClient(SOCKET i)
{
	int result = send(i, dataToSend, sizeof(dataToSend), 0);
	if (result == SOCKET_ERROR)
	{
		std::cout << "send failed(server)\n" << std::to_string(result) << "\n";
		std::cout << WSAGetLastError();
	}
}
void Server::RegisterNewPlayer()
{
	if (playerCount >= maxPlayerCount)
	{
		answerCode = (int)JoinAnswerFailed;
		return;
	}
	if (playerData.size() == 0)
	{
		//std::cout << "PlayerData size = 0\n";
		currentPlayerID = 0;
	}
	else
	{
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

int Server::InitServer()
{
	OpenDebugConsole();
	if (WSAStartup(MAKEWORD(2, 2), &d)) {
		return -1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* bindAddress;
	if (getaddrinfo("127.0.0.1", "8080", &hints, &bindAddress) != 0) {
		WSACleanup();
		return -1;
	}
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET) {
		freeaddrinfo(bindAddress);
		WSACleanup();
		return -1;
	}
	u_long mode = 1;
	if (ioctlsocket(listenerSocket, FIONBIO, &mode) != 0) {
		closesocket(listenerSocket);
		freeaddrinfo(bindAddress);
		WSACleanup();
		return -1;
	}
	if (bind(listenerSocket, bindAddress->ai_addr, static_cast<int>(bindAddress->ai_addrlen)) != 0) {
		closesocket(listenerSocket);
		freeaddrinfo(bindAddress);
		WSACleanup();
		return -1;
	}
	freeaddrinfo(bindAddress);
	if (listen(listenerSocket, 10) < 0) {
		closesocket(listenerSocket);
		WSACleanup();
		return -1;
	}
	maxSocket = listenerSocket;
	FD_ZERO(&master);
	FD_SET(listenerSocket, &master);
	while (true)
	{
		UpdateServer();
	}
	return 0;
}

void Server::UpdateServer()
{
	fd_set reads;
	FD_ZERO(&reads);
	reads = master;

	struct timeval timeout;
	timeout.tv_sec = 1;  // 1 second timeout
	timeout.tv_usec = 0;
	int selectResult = select(static_cast<int>(maxSocket + 1), &reads, nullptr, nullptr, &timeout);
	if (selectResult < 0) {
		int error = WSAGetLastError();
		std::string errorMsg = "select() failed with error code: " + std::to_string(error);
		return;
	}
	else if (selectResult == 0) {
		//// Timeout occurred, no sockets ready
		//UnserDebugFunktionoenchen("select() timeout, no sockets ready");
		//return;
	}
	for (SOCKET i = 0; i <= maxSocket; i++) {
		if (FD_ISSET(i, &reads)) {
			if (i == listenerSocket) {
				SOCKET clientSocket = accept(listenerSocket, nullptr, nullptr);
				if (clientSocket == INVALID_SOCKET) {
					int error = WSAGetLastError();
					std::string errorMsg = "accept() failed with error code: " + std::to_string(error);
				}
				else {
					FD_SET(clientSocket, &master);
					if (static_cast<int>(clientSocket) > maxSocket) {
						maxSocket = clientSocket;
					}
				}
			}
			else {
				int bytesReceived = recv(i, request, sizeof(request), 0);
				float temp[5];
				memcpy(&temp, request, sizeof(temp));
				if (bytesReceived <= 0) {
					closesocket(i);
					FD_CLR(i, &master);
					if (i == maxSocket) {
						while (FD_ISSET(maxSocket, &master) == false) {
							maxSocket--;
						}
					}
				}
				else {
					HandleIncomingRequest(i);
				}
			}
		}
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
void Server::OpenDebugConsole()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
	freopen_s(&file, "CONOUT$", "w", stderr);
	freopen_s(&file, "CONIN$", "r", stdin);
	std::cout << "Debug-Konsole gestartet." << std::endl;
}
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