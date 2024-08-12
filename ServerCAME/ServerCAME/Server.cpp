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
	, playerData{ {0, Position(100, 400000000, 32422420)} } // Verwende eine Initialisierungsliste für `std::map`
	, playerCount(0)
	, currentPlayerID(0)
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

void Server::SendToClient(SOCKET i, std::string msg)
{
	char formatedAnswer[4096];
	memcpy(formatedAnswer, msg.data(), msg.size());
	formatedAnswer[msg.size()] = '\0';
	send(i, formatedAnswer, msg.size() + 1, 0);
}

void Server::HandleIncomingRequest(bool& readingRequest, SOCKET i) 
{
	int msgCode;
	memcpy(&msgCode, request, sizeof(msgCode));
	switch(msgCode)
	{
	case 101:
		RegisterNewPlayer(i); 
		break; 
	case '102': 
		UpdatePlayerPosition(i); 
		break; 
	default: 
		printf("Unhandelt request"); 
		break; 
	}
}

void Server::ReadMessage(char* message)
{
	int msgCode;
	memcpy(&msgCode, &message[0], sizeof(msgCode));

	int PlayerId;
	memcpy(&PlayerId, &message[4], sizeof(PlayerId));
	float PosX, PosY, PosZ;
	memcpy(&PosX, &message[8], sizeof(PosX));
	memcpy(&PosY, &message[12], sizeof(PosY));
	memcpy(&PosZ, &message[16], sizeof(PosZ));
	std::cout << "Message Code: " << msgCode << std::endl;
	std::cout << "Player ID: " << PlayerId << std::endl;
	std::cout << "Position X: " << PosX << std::endl;
	std::cout << "Position Y: " << PosY << std::endl;
	std::cout << "Position Z: " << PosZ << std::endl;
}

void Server::RegisterNewPlayer(SOCKET i)
{
	playerData.insert(std::make_pair(playerCount, Position(startPosOffset, startPosOffset, startPosOffset)));
	int msgCode = (int)JoinAwnserSucessful; 
	std::string msg = std::to_string(msgCode)
	+ std::to_string(playerCount)
	+ ServerUttilitys::FloatToString(playerData[playerCount].x)
	+ ServerUttilitys::FloatToString(playerData[playerCount].y) 
	+ ServerUttilitys::FloatToString(playerData[playerCount].z);
	startPosOffset += 10;
	playerCount++;
	SendToClient(i, msg); 
}

void Server::RegisterNewPlayer()
{
	// '/' => end of parameter, '|' => end of message
	playerData.insert(std::make_pair(playerCount, Position(startPosOffset, startPosOffset, startPosOffset)));
	playerCount++;
}
void Server::UpdatePlayerPosition(SOCKET i)
{

}


std::string Server::PrepareMessage(protocol protocolCode)
{
	char message[20];

	int code = static_cast<int>(protocolCode);
	memcpy(&message[0], &code, sizeof(code));

	int playerId = currentPlayerID;
	memcpy(&message[4], &playerId, sizeof(playerId));

	float posX = playerData[currentPlayerID].x;
	float posY = playerData[currentPlayerID].y;
	float posZ = playerData[currentPlayerID].z;

	memcpy(&message[8], &posX, sizeof(posX)); 
	memcpy(&message[12], &posY, sizeof(posY));
	memcpy(&message[16], &posZ, sizeof(posZ));
	
	return (std::string)message;
}

int Server::InitServer(int argc, char* argv[])
{
	ReadMessage();  
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
	SOCKET listenerSocket;
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
		if (select(maxSocket + 1, &reads, 0, 0, 0) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
			return -1;
		}
		SOCKET i;
		for (i = 0; i <= maxSocket; i++)
		{
			if (FD_ISSET(i, &reads))
			{
				if (i == listenerSocket)
				{
					sockaddr_storage client;
					socklen_t clientLegth = sizeof(client);
					SOCKET clientSocket = accept(listenerSocket, reinterpret_cast<sockaddr*>(&client), &clientLegth);

					if (clientSocket == INVALID_SOCKET)
					{
						fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
						return -1;
					}
					FD_SET(clientSocket, &master);
					if (clientSocket > maxSocket)
					{
						maxSocket = clientSocket;
					}
					char adressBuffer[100];
					getnameinfo(reinterpret_cast<sockaddr*>(&client), clientLegth, adressBuffer, sizeof(adressBuffer), 0, 0, NI_NUMERICHOST);
					printf("New connection from: %s\n", adressBuffer);
				}
				else
				{
					int bytesReceived = recv(i, request, sizeof(request), 0);
					if (bytesReceived > 0) {
						HandleIncomingRequest(readingRequest, i);
					}
					else if (bytesReceived == 0) {
						// Verbindung geschlossen
						FD_CLR(i, &master);
						closesocket(i);
					}
					else {
						// Fehler beim Empfangen von Daten
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