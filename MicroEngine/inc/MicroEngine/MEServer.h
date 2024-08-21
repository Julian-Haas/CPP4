#pragma once
#include <iostream>
#include<vector>
#include <string>
#include <map>
#include<array>
#include<Windows.h>
#include<fstream>
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
#include <array>
struct Position
{
public:
	SOCKET playersocket;
	float x;
	float y;
	float z;
	Position() = default;
	Position(SOCKET socket, float xPos, float yPos, float zPos)
		: playersocket(socket)
		, x(xPos)
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
	bool _debugFlag = false;
	//protocol enum 
	float recievedFloats[5];
	float SendedPositions[3];
	char dataToSend[20];
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
	SOCKET currentPlayerSocket;
	SOCKET maxSocket;
	fd_set master;
	addrinfo hints;
	WSAData d;
	//member functions: 
	void SendToClient(SOCKET i)
	{
		int result = send(i, dataToSend, sizeof(dataToSend), 0);
		if (result == SOCKET_ERROR)
		{
			std::cout << "send failed(server)\n" << std::to_string(result) << "\n";
			std::cout << WSAGetLastError();
			//UnserDebugFunktionoenchen(message); 
		}
	}
	void RegisterNewPlayer()
	{
		if (playerCount >= maxPlayerCount)
		{
			//std::cout << "Player count too high" << playerCount << std::endl;
			answerCode = (int)JoinAwnserFailed;
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

		playerData.insert(std::make_pair(currentPlayerID, Position(currentPlayerSocket, 0, 0, 30)));
		playerCount++;
		answerCode = (int)JoinAwnserSucessful;
	}

	void OpenDebugConsole()
	{
		// Neue Konsole erzeugen
		AllocConsole();

		// Konsole für Standardausgabe einrichten
		FILE* file;
		freopen_s(&file, "CONOUT$", "w", stdout);
		freopen_s(&file, "CONOUT$", "w", stderr);
		freopen_s(&file, "CONIN$", "r", stdin);

		std::cout << "Debug-Konsole gestartet." << std::endl;
	}
	void PrepareMessage()
	{
		float x[5];
		x[0] = answerCode;
		x[1] = currentPlayerID;
		x[2] = playerData[currentPlayerID].x;
		x[3] = playerData[currentPlayerID].y;
		x[4] = playerData[currentPlayerID].z++;
		memcpy(&dataToSend, x, sizeof(dataToSend));
	}
	SOCKET listenerSocket;
	void UpdatePlayerPosition()
	{
		// Implement as needed
	}
	void PrintFloats(float x[])
	{
		for (int i = 0; i < 5; i++)
		{
			std::cout << "Element: " << " " << i << " " << x[i] << " ";
		}
		std::cout << "\n";
	}
	void HandleIncomingRequest(SOCKET i)
	{
		//float temp[5];
		//memcpy(&temp, request, sizeof(temp));
		//std::cout << temp[0] << "\n";

		//if (_debugFlag == false) {
		//	for (int i = 0; i < 5; i++) {
		//		//std::cout << std::to_string(temp[i]) << "\n";
		//		std::cout << std::to_string(temp[i]) << "\n";
		//		//std::cout << std::to_string((byte)unformattedRequest[i]) << "\n";
		//	}
		//	_debugFlag = true;
		//}

		memcpy(&recievedFloats, request, sizeof(recievedFloats));
		int msgCode = (int)recievedFloats[0];
		currentPlayerID = (int)recievedFloats[1];
		SOCKET maxSocket = listenerSocket;

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
			//UpdatePlayerPosition();  // Annahme: Diese Funktion existiert
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

			// Unbehandelter Nachrichtencode
			std::cout << "Unhandled request: " << msgCode << std::endl;
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
		OpenDebugConsole();
		if (WSAStartup(MAKEWORD(2, 2), &d)) {
			//UnserDebugFunktionoenchen("WinSocket failed to initialize");
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
			//UnserDebugFunktionoenchen("getaddrinfo() failed");
			WSACleanup();
			return -1;
		}
		//UnserDebugFunktionoenchen("Creating listener socket");

		listenerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);
		if (listenerSocket == INVALID_SOCKET) {
			//UnserDebugFunktionoenchen("socket() failed");
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}

		u_long mode = 1; // Make socket non-blocking



		if (ioctlsocket(listenerSocket, FIONBIO, &mode) != 0) {
			//UnserDebugFunktionoenchen("ioctlsocket() failed");
			closesocket(listenerSocket);
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}

		//UnserDebugFunktionoenchen("Binding address to socket");
		if (bind(listenerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen) != 0) {
			//UnserDebugFunktionoenchen("bind() failed");
			closesocket(listenerSocket);
			freeaddrinfo(bindAddress);
			WSACleanup();
			return -1;
		}
		freeaddrinfo(bindAddress);

		//UnserDebugFunktionoenchen("Listening for connections...");
		if (listen(listenerSocket, 10) < 0) {
			//UnserDebugFunktionoenchen("listen() failed");
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

		//UnserDebugFunktionoenchen("Server initialization successful");

		return 0;
	}


	void UnserDebugFunktionoenchen(int a) {
		std::string resultStr = std::to_string(listenerSocket);
		const char* resultCStr = resultStr.c_str();

		//ME_LOG_ERROR(resultCStr);
	}
	void UnserDebugFunktionoenchen(std::string a) {
		std::cout << a << std::endl;
		//ME_LOG_ERROR(resultCStr);
	}

	void UpdateServer() {
		fd_set reads;
		FD_ZERO(&reads);
		reads = master;

		struct timeval timeout;
		timeout.tv_sec = 1;  // 1 second timeout
		timeout.tv_usec = 0;
		int selectResult = select(maxSocket + 1, &reads, nullptr, nullptr, &timeout);

		if (selectResult < 0) {
			int error = WSAGetLastError();
			std::string errorMsg = "select() failed with error code: " + std::to_string(error);
			//UnserDebugFunktionoenchen(errorMsg.c_str());
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
					// New connection
					SOCKET clientSocket = accept(listenerSocket, nullptr, nullptr);
					if (clientSocket == INVALID_SOCKET) {
						int error = WSAGetLastError();
						std::string errorMsg = "accept() failed with error code: " + std::to_string(error);
						//UnserDebugFunktionoenchen(errorMsg.c_str());
					}
					else {
						FD_SET(clientSocket, &master);
						if (clientSocket > maxSocket) {
							maxSocket = clientSocket;
						}
					}
				}
				else {
					int bytesReceived = recv(i, request, sizeof(request), 0);

					float temp[5];
					memcpy(&temp, request, sizeof(temp));
					if (bytesReceived <= 0) {
						// Client disconnected or error occurred
						closesocket(i);
						FD_CLR(i, &master);
						if (i == maxSocket) {
							// Recalculate maxSocket
							while (FD_ISSET(maxSocket, &master) == false) {
								maxSocket--;
							}
						}
					}
					else {
						// Process received data
						HandleIncomingRequest(i);
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));  // Slight delay to avoid CPU overuse
	}

	void CloseClientSocket(SOCKET clientSocket) {
		closesocket(clientSocket);
		FD_CLR(clientSocket, &master);
		//UnserDebugFunktionoenchen("Closed and removed socket from master set");
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