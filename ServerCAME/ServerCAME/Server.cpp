#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <iostream>
#include<chrono>
#include "Server.h"
#include <optional>
#include "CAServerUttilitys.h"
Server::Server()
	: startPosOffset(1000243.3F)
	, playerCount(0)
	, currentPlayerID(0)
	, playerData{ {-1, Position(0, 0, 0)} }
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
	switch (msgCode)
	{
	case 101:
		RegisterNewPlayer();
		auto message = PrepareMessage();
		SendToClient(i, message.data());
		for (n = 0; n <= maxSocket; n++)
		{
			if (n != i)
			{
				msgCode = (int)ProceedData;
				SendToClient(n, message.data());
			}
		}
		break;
	case '102':
		//maybe is irrelavnt UpdatePlayerPosition(); 
		break;
	default:
		printf("Unhandelt request");
		break;
	}
}

void Server::ReadMessage(const char* message)
{
	int msgCode;
	memcpy(&requestCode, &message[0], sizeof(requestCode));

	int PlayerId;
	memcpy(&currentPlayerID, &message[4], sizeof(currentPlayerID));

	float PosX, PosY, PosZ;
	memcpy(&playerData[currentPlayerID].x, &message[8], sizeof(playerData[currentPlayerID].x));
	memcpy(&playerData[currentPlayerID].y, &message[12], sizeof(playerData[currentPlayerID].y));
	memcpy(&playerData[currentPlayerID].z, &message[16], sizeof(playerData[currentPlayerID].z));

	std::cout << "Message Code: " << requestCode << std::endl;
	std::cout << "Player ID: " << currentPlayerID << std::endl;
	std::cout << "Position X: " << playerData[currentPlayerID].x << std::endl;
	std::cout << "Position Y: " << playerData[currentPlayerID].y << std::endl;
	std::cout << "Position Z: " << playerData[currentPlayerID].z << std::endl;
}

void Server::RegisterNewPlayer()
{
	if (playerCount >= maxPlayerCount)
	{
		answerCode = (int)JoinAwnserFailed;
		return;
	}

	if (playerData.size() > 0)
	{
		auto it = playerData.begin();
		int i = 0;
		while (it != playerData.end())
		{
			std::cout << "Vergleiche items\n";
			if (it->first != i)
			{
				currentPlayerID = i;
				break;
			}
			it++;
			i++;
		}
	} else 
	{
		currentPlayerID = 0;
		playerCount = 0; 
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

	//Debuging ausgabe
	for (auto it = playerData.begin(); it != playerData.end(); ++it)
	{
		std::cout << "Key: " << it->first << ", Value: (" << it->second.x << ", " << it->second.y << ", " << it->second.z << ")" << std::endl;
	}
	playerCount--;
}

void Server::UpdatePlayerPosition()
{

}


std::array<char, 20> Server::PrepareMessage()
{
	std::array<char, 20> message = {};  // Initialisiere mit 0

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

	ReadMessage(message.data()); // Verwende .data() um einen Zeiger zu erhalten
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
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo* bindAddress;
	getaddrinfo(0, "8080", &hints, &bindAddress);
	printf("Creating listener socket\n");
	listenerSocket;
	listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
	if (listenerSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	u_long mode = 1; // 
	ioctlsocket(listenerSocket, FIONBIO, &mode); // makro das den listener zu nicht blockierendem makro macht
	printf("Binding address to socket\n");
	if (bind(listenerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen))
	{
		fprintf(stderr, "bind() failed. (%d)\n", WSAGetLastError());
		return -1;
	}
	freeaddrinfo(bindAddress);
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
	//server loop
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
        return -1;
    }
    else if (selectResult == 0)
    {
        // Timeout occurred, no activity detected
        continue; // Continue the loop, handle other tasks here if needed
    }

    SOCKET i;
    for (i = 0; i <= maxSocket; i++)
    {
        if (FD_ISSET(i, &reads))
        {
            if (i == listenerSocket)
            {
                sockaddr_storage client;
                socklen_t clientLength = sizeof(client);
                SOCKET clientSocket = accept(listenerSocket, reinterpret_cast<sockaddr*>(&client), &clientLength);

                if (clientSocket == INVALID_SOCKET)
                {
                    fprintf(stderr, "accept() failed. (%d)\n", WSAGetLastError());
                    return -1;
                }
                FD_SET(clientSocket, &master);
                if (clientSocket > maxSocket)
                {
                    maxSocket = clientSocket;
                }
                char addressBuffer[100];
                getnameinfo(reinterpret_cast<sockaddr*>(&client), clientLength, addressBuffer, sizeof(addressBuffer), 0, 0, NI_NUMERICHOST);
                printf("New connection from: %s\n", addressBuffer);
            }
            else
            {
                int bytesReceived = recv(i, request, sizeof(request), 0);
                if (bytesReceived > 0) {
                    HandleIncomingRequest(readingRequest, i);
                }
                else if (bytesReceived == 0) {
                    FD_CLR(i, &master);
                    closesocket(i);
                }
                else {
                    fprintf(stderr, "recv() failed. (%d)\n", WSAGetLastError());
                    FD_CLR(i, &master);
                    closesocket(i);
                }
            }
        }
    }
}

	closesocket(listenerSocket);
	WSACleanup();
	return 0;
}