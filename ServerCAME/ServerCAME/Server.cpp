#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include "Server.h"
#include <optional>
#include "CAServerUttilitys.h"

Server::Server()
	: startPosOffset(1000243.3F)
	, playerCount(0)
	, currentPlayerID(0)
	, requestCode(0)
	, answerCode(0)
	, maxPlayerCount(2)
{
}

enum Server::protocol
{
	JoinAwnserSucessful = 1,
	JoinAwnserFailed = 2,
	ProceedData = 3,
	JoinRequest = 101,
	SendPosition = 102,
};

void Server::SendToClient(SOCKET i, const char* msg)
{
	send(i, msg, 20, 0);
}

void Server::HandleIncomingRequest(bool& readingRequest, SOCKET i)
{
	ReadMessage(request);
	int msgCode;
	memcpy(&msgCode, request, sizeof(msgCode));
	SOCKET maxSocket = listenerSocket;
	SOCKET n;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listenerSocket, &master);

	switch (sendedInts[0])
	{
	case 101:
		RegisterNewPlayer();
		auto message = PrepareMessage();
		SendToClient(i, message.data());
		for (n = 0; n <= maxSocket; n++)
		{
			if (n != i && FD_ISSET(n, &master))
			{
				msgCode = (int)ProceedData;
				SendToClient(n, message.data());
			}
		}
		break;
	case 102:
		// maybe is irrelevant UpdatePlayerPosition(); 
		break;
	default:
		printf("Unhandled request");
		break;
	}
}

void Server::ReadMessage(const char* message)
{
	int msgCode;
	memcpy(sendedInts, message, sizeof(sendedInts));
	memcpy(SendedPositions, message + 8, sizeof(SendedPositions));

	std::cout << "Message Code: " << sendedInts[0] << std::endl;
	std::cout << "Player ID: " << sendedInts[1] << std::endl;
	std::cout << "Position X: " << SendedPositions[0] << std::endl;
	std::cout << "Position Y: " << SendedPositions[1] << std::endl;
	std::cout << "Position Z: " << SendedPositions[2] << std::endl;
}

void Server::RegisterNewPlayer()
{
	if (playerCount >= maxPlayerCount)
	{
		std::cout << "Player count too high" << playerCount << std::endl;
		answerCode = (int)JoinAwnserFailed;
		return;
	}

	if (playerData.size() == 0)
	{
		std::cout << "PlayerData size = 0\n";
		currentPlayerID = 0;
	}
	else
	{
		auto it = playerData.begin();
		int newID = -1;
		int i = 0;

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

		if (newID == -1)
		{
			auto lastKey = std::prev(playerData.end());
			newID = lastKey->first + 1;
		}

		currentPlayerID = newID;
	}

	playerData.insert(std::make_pair(currentPlayerID, Position(startPosOffset, startPosOffset, startPosOffset)));
	playerCount++;
	answerCode = (int)JoinAwnserSucessful;
}

void Server::UnregisterPlayer()
{
	for (int i = 0; i < 13; ++i)
	{
		playerData.insert(std::make_pair(i, Position(10, 10, 10)));
	}

	currentPlayerID = 4;
	int indexToRemove = currentPlayerID;

	auto it = playerData.begin();
	playerData[0].x = -5;
	playerData[0].y = -5;
	playerData[0].z = -5;
	while (it != playerData.end())
	{
		if (it->first > indexToRemove)
		{
			int newKey = it->first - 1;
			playerData[newKey] = it->second;
			it = playerData.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Debug output
	for (auto it = playerData.begin(); it != playerData.end(); ++it)
	{
		std::cout << "Key: " << it->first << ", Value: (" << it->second.x << ", " << it->second.y << ", " << it->second.z << ")" << std::endl;
	}
	playerCount--;
}

void Server::UpdatePlayerPosition()
{
	// Implement as needed
}

std::array<char, 20> Server::PrepareMessage()
{
	std::array<char, 20> message = {};  // Initialize with 0

	int code = answerCode;
	memcpy(&message[0], &code, sizeof(code));

	int playerId = currentPlayerID;
	memcpy(&message[4], &playerId, sizeof(playerId));

	float posX = playerData[currentPlayerID].x;
	float posY = playerData[currentPlayerID].y;
	float posZ = playerData[currentPlayerID].z;

	memcpy(&message[8], &posX, sizeof(posX));
	memcpy(&message[12], &posY, sizeof(posY));
	memcpy(&message[16], &posZ, sizeof(posZ));

	ReadMessage(message.data()); // Use .data() to get a pointer
	return message;
}

int Server::InitServer(int argc, char* argv[])
{
	ReadMessage(request);
	WSAData d;
	bool readingRequest = false;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("WinSocket failed to initialize\n");
		return -1;
	}
	printf("Configuring local ip address\n");
	sockaddr_in service;
	int opt = 1;
	listenerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenerSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
		fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	service.sin_family = AF_INET;
	service.sin_port = htons(5000);
	service.sin_addr.s_addr = INADDR_ANY;//inet_addr("178.203.204.116");
	//InetPton(AF_INET, __TEXT("178.203.204.116"), &service.sin_addr.s_addr);
	printf("Creating listener socket\n");

	//u_long mode = 1; // 
	//ioctlsocket(listenerSocket, FIONBIO, &mode);
	printf("Binding address to socket\n");
	int result = bind(listenerSocket, (SOCKADDR*)&service, sizeof(service));

	if (result == SOCKET_ERROR)
	{
		fprintf(stderr, "bind() failed. (%d)\n", WSAGetLastError());
		closesocket(listenerSocket);
		WSACleanup();
		return -1;
	}

	printf("Listening...\n");
	if (listen(listenerSocket, 10) < 0)
	{
		fprintf(stderr, "listen() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listenerSocket, &master);
	SOCKET maxSocket = listenerSocket;

	// Server loop
	while (true)
	{
		fd_set reads = master;

		// Set a timeout value
		struct timeval timeout;
		timeout.tv_sec = 1;  // 1 second timeout
		timeout.tv_usec = 0;

		int selectResult = select(maxSocket + 1, &reads, 0, 0, &timeout);

		if (selectResult < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
			continue;  // Handle error and continue loop
		}
		else if (selectResult == 0)
		{
			// Timeout, no action needed, continue loop
			continue;
		}

		for (SOCKET i = 0; i <= maxSocket; i++)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == listenerSocket)
				{
					// New connection
					SOCKET clientSocket = accept(listenerSocket, NULL, NULL);
					if (clientSocket == INVALID_SOCKET)
					{
						fprintf(stderr, "accept() failed. (%d)\n", WSAGetLastError());
					}
					else
					{
						printf("New connection from %s\n", GetClientIP(clientSocket).c_str());

						// Check client IP and only accept if it matches
						//if (GetClientIP(clientSocket) == "0.0.0.0")  // Chat-GPT: Restrict to specific IP
						//{
						FD_SET(clientSocket, &master);
						if (clientSocket > maxSocket)
						{
							maxSocket = clientSocket;
						}
						//}
						//else
						//{
						//	printf("Rejected connection from %s\n", GetClientIP(clientSocket).c_str());
						//	closesocket(clientSocket);
						//}
					}
				}
				else
				{
					// Handle data from an existing client
					HandleIncomingRequest(readingRequest, i);
				}
			}
		}
	}

	return 0;
}

std::string Server::GetClientIP(SOCKET clientSocket)
{
	sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	if (getpeername(clientSocket, (sockaddr*)&clientAddr, &addrLen) == 0)
	{
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
		return std::string(ipStr);
	}
	return "";
}
