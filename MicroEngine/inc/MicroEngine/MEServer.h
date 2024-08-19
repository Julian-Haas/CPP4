#pragma once
#include <iostream>
#include<vector>
#include <string>
#include <map>
#include<array>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <chrono>
#include <optional>
#include "MEServerHelper.h"
#include <thread>
struct Position
{
public:
	float x;
	float y;
	float z;
	Position() {}
	Position(float xPos, float yPos, float zPos)
		: x(xPos)
		, y(yPos)
		, z(zPos)
	{}

	~Position()
	{}
};

#pragma once
class Server
{
private:
	//protocol enum 
	float sendedInts[5];
	float SendedPositions[3];
	enum protocol
	{
		JoinAwnserSucessful = 1,
		JoinAwnserFailed = 2,
		ProceedData = 3,
		JoinRequest = 101,
		SendPosition = 102,
	};
	char request[20];
	std::map<int, Position> playerData;
	float startPosOffset;
	int playerCount;
	int currentPlayerID;
	int requestCode;
	int answerCode;
	int maxPlayerCount = 2;
	SOCKET maxSocket;
	fd_set master;
	addrinfo hints;
	WSAData d;
	//member functions: 
	void SendToClient(SOCKET i, const char* msg)
	{
		send(i, msg, 20, 0);
	}
	void ReadMessage(const char* message)
	{
		int msgCode;
		memcpy(sendedInts, message, sizeof(sendedInts));
	}
	void RegisterNewPlayer()
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
	std::array<char, 20> PrepareMessage()
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
	SOCKET listenerSocket;
	void UpdatePlayerPosition()
	{
		// Implement as needed
	}

	void HandleIncomingRequest(SOCKET i)
	{
		int bytesRecieved = recv(i, request, sizeof(request), 0); 
		ReadMessage(request);
		int msgCode;
		memcpy(&msgCode, request, sizeof(msgCode));
		SOCKET maxSocket = listenerSocket;
		SOCKET n;
		fd_set master;
		FD_ZERO(&master);
		FD_SET(listenerSocket, &master);
		switch ((int)sendedInts[0])
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
			for (n = 0; n <= maxSocket; n++)
			{

				Beep(750, 300);
				msgCode = (int)ProceedData;
				SendToClient(n, message.data());
				
			}
			break;
		default:
			printf("Unhandled request");
			break;
		}
	}

public:
	Server()
		: startPosOffset(1000243.3F)
		, playerCount(0)
		, currentPlayerID(0)
		, requestCode(0)
		, answerCode(0)
		, maxPlayerCount(2)
	{
	}
	void UnregisterPlayer()
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
	int InitServer() {
		//ReadMessage(request);

		if (WSAStartup(MAKEWORD(2, 2), &d)) {
			UnserDebugFunktionoenchen("WinSocket failed to initialize");
			return -1;
		}
		//UnserDebugFunktionoenchen("Configuring local IP address");

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;
		addrinfo* bindAddress;

		// Bind to specific IP address and port 8080
		if (getaddrinfo("127.0.0.1", "8080", &hints, &bindAddress) != 0) {
			UnserDebugFunktionoenchen("getaddrinfo() failed");
			WSACleanup();
			return -1;
		}
		//UnserDebugFunktionoenchen("Creating listener socket");

		listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
		if (listenerSocket == INVALID_SOCKET) {
			UnserDebugFunktionoenchen("socket() failed");
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}

		u_long mode = 1; // Make socket non-blocking
		if (ioctlsocket(listenerSocket, FIONBIO, &mode) != 0) {
			UnserDebugFunktionoenchen("ioctlsocket() failed");
			closesocket(listenerSocket);
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}

		//UnserDebugFunktionoenchen("Binding address to socket");
		if (bind(listenerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen) != 0) {
			UnserDebugFunktionoenchen("bind() failed");
			closesocket(listenerSocket);
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}
		freeaddrinfo(bindAddress);

		//UnserDebugFunktionoenchen("Listening for connections...");
		if (listen(listenerSocket, 10) < 0) {
			UnserDebugFunktionoenchen("listen() failed");
			closesocket(listenerSocket);
			WSACleanup();
			return -1;
		}

		maxSocket = listenerSocket;
		FD_ZERO(&master);
		FD_SET(listenerSocket, &master);
		while(true)
		{
			UpdateServer(); 
		}

		//UnserDebugFunktionoenchen("Server initialization successful");

		return 0;
	}


	void UnserDebugFunktionoenchen(int a) {
		std::string resultStr = std::to_string(listenerSocket);
		const char* resultCStr = resultStr.c_str();

		ME_LOG_ERROR(resultCStr);
	}
	void UnserDebugFunktionoenchen(std::string a) {
		const char* resultCStr = a.c_str();

		ME_LOG_ERROR(resultCStr);
	}

	void UpdateServer() {
		fd_set reads;
		FD_ZERO(&reads);
		reads = master;

		struct timeval timeout;
		timeout.tv_sec = 1;  // 1 second timeout
		timeout.tv_usec = 0;


		// Überprüfe, ob alle Sockets in master gültig sind
		for (SOCKET i = 0; i <= maxSocket; i++) {
			if (FD_ISSET(i, &master)) {
				if (i == INVALID_SOCKET) {
					UnserDebugFunktionoenchen("Invalid socket found in master set");
					FD_CLR(i, &master);  // Entferne den ungültigen Socket
				}
			}
		}

		int selectResult = select(maxSocket + 1, &reads, nullptr, nullptr, &timeout);


		if (selectResult < 0) {
			int error = WSAGetLastError();
			std::string errorMsg = "select() failed with error code: " + std::to_string(error);
			UnserDebugFunktionoenchen(errorMsg.c_str());
			return;
		}
		else if (selectResult == 0) {
			// Timeout occurred, no sockets ready
			UnserDebugFunktionoenchen("select() timeout, no sockets ready");
			return;
		}

		for (SOCKET i = 0; i <= maxSocket; i++) {
			if (FD_ISSET(i, &reads)) {
				int bytesReceived = recv(listenerSocket, request, sizeof(request), 0);
				if (i == listenerSocket) {
					// New connection
					SOCKET clientSocket = accept(listenerSocket, nullptr, nullptr);
					if (clientSocket == INVALID_SOCKET) {
						int error = WSAGetLastError();
						std::string errorMsg = "accept() failed with error code: " + std::to_string(error);
						UnserDebugFunktionoenchen(errorMsg.c_str());
					}
					else {
						//std::string connectionMsg = "New connection from socket: " + std::to_string(clientSocket);
						Beep(750, 300); 
						//UnserDebugFunktionoenchen(connectionMsg.c_str());

						FD_SET(clientSocket, &master);
						if (clientSocket > maxSocket) {
							maxSocket = clientSocket;
						}
					}
				}
				else {
					// Handle data from an existing client
					HandleIncomingRequest(i);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));  // Slight delay to avoid CPU overuse
	}
	void CloseClientSocket(SOCKET clientSocket) {
		closesocket(clientSocket);
		FD_CLR(clientSocket, &master);
		UnserDebugFunktionoenchen("Closed and removed socket from master set");
	}



	std::string GetClientIP(SOCKET clientSocket)
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

};