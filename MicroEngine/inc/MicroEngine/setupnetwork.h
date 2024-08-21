#pragma once
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")
#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "MicroEngine\MEServer.h"
namespace me {
	enum protocol
	{
		RequestJoin_Code = 101,
		SendPosition_Code = 102,
		SendLogOut_Code = 103
	};
	class SetupNetwork {
	private:
		SOCKET serverSocket;
		float unformattedRequest[5];
		char formattedRequest[20];
		int receivedMessageInInt[5];
		char receivedMessage[20];
		enum protocol;
		int _playerID = -1;
		float _position_x = 3;
		float _position_y = 4;
		float _position_z = 5;
		Server server;
	public:
		SetupNetwork()
			: server()
			, _playerID(0)
			, _position_x(0)
			, _position_y(0)
			, _position_z(30)
		{
		}
		void UpdateTheServer()
		{
			server.UpdateServer();
		}
		bool SetupNetwork::ReadData(float* dataStorage, int& playerID, int& selectedPlayerID)
		{
			bool readAllData = false;
			while (!readAllData)
			{
				fd_set reads;
				FD_ZERO(&reads);
				FD_SET(serverSocket, &reads);

				timeval timeout;
				timeout.tv_sec = 0;
				timeout.tv_usec = 50000;  // Erhöhe den Timeout-Wert auf 50ms
				int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
				int selectResult = select(serverSocket + 1, &reads, NULL, NULL, &timeout);
				if (selectResult == -1) {
					std::cout << "select failed\n"; 
					return false; 
				}
				if (selectResult == 0) {
					std::cout << "timeout\n";
					return false;
				}
				if (selectResult > 0 && FD_ISSET(serverSocket, &reads))
				{
					if (bytesReceived <= 0)
					{
						return false;
					}
					memcpy(&receivedMessageInInt, receivedMessage, sizeof(receivedMessageInInt));
					memcpy(&receivedMessageInInt, receivedMessage, sizeof(receivedMessageInInt));

					switch (receivedMessageInInt[0])
					{
					case 1:
						_playerID = receivedMessageInInt[1];
						playerID = _playerID;
						selectedPlayerID = _playerID;
						break;
					case 2:
						return false;
					case 3:
						if (receivedMessageInInt[1] != _playerID)
						{
							selectedPlayerID = receivedMessageInInt[1];
						}
						break;
					default:
						return false;  // Ungültige Nachricht empfangen, Schleife abbrechen
					}
				}
				else
				{
					return false;  // Timeout ohne Daten, Schleife abbrechen
				}
			}
			return true;
		}


		bool SetupNetwork::SearchForServer()
		{
			//magic numbers
			const char* serverIP = "127.0.0.1";
			const char* serverPort = "8080";

			bool sucess = false;

			WSAData d;
			if (WSAStartup(MAKEWORD(2, 2), &d))
			{
				printf("WinSocket failed to initialize\n");
				return false;
			}
			printf("Configuring remote address\n");
			addrinfo hints;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			addrinfo* server;
			if (getaddrinfo(serverIP, serverPort, &hints, &server))
			{
				fprintf(stderr, "getaddrinfo() failed. (%d)\n", WSAGetLastError());
				return false;
			}
			printf("Remote address is :\n");
			PCHAR addressBuffer = nullptr;
			getnameinfo(server->ai_addr, (socklen_t)server->ai_addrlen, addressBuffer, sizeof(addressBuffer), 0, 0, NI_NUMERICHOST);
			printf("%s\n", addressBuffer);
			printf("Creating socket...\n");
			serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
			if (serverSocket == INVALID_SOCKET)
			{
				fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
				return false;
			}

			// Set non-blocking mode
			u_long mode = 1;
			ioctlsocket(serverSocket, FIONBIO, &mode);

			printf("Connecting to server...\n");
			int result = connect(serverSocket, server->ai_addr, (int)server->ai_addrlen);
			if (result == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					fd_set writeSet;
					FD_ZERO(&writeSet);
					FD_SET(serverSocket, &writeSet);

					timeval timeout;
					timeout.tv_sec = 1;
					timeout.tv_usec = 0;

					result = select(0, NULL, &writeSet, NULL, &timeout);
					if (result > 0)
					{
						printf("Connected!\n");
						sucess = true;
					}
					else if (result == 0)
					{
						printf("Connection timed out.\n");
					}
					else
					{
						fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
					}
				}
				else
				{
					fprintf(stderr, "connect() failed. (%d)\n", error);
				}
			}
			freeaddrinfo(server);
			return sucess;
		}
		void SetupNetwork::BeepBeep() {
			Beep(750, 300);
		}
		void SetupNetwork::SendMessageToServer(int code) {
			//_playerID = 1;
			//_position_x = 12.34f;
			//_position_y = 56.78f;
			//_position_z = 90.12f;
		   
			unformattedRequest[0] = code;
			unformattedRequest[1] = _playerID;
			unformattedRequest[2] = _position_x;
			unformattedRequest[3] = _position_y;
			unformattedRequest[4] = _position_z;

			//test1
			//std::ostringstream stream;
			//stream << _playerID << " "
			//	<< std::fixed << std::setprecision(2)
			//	<< _position_x << " "
			//	<< _position_y << " "
			//	<< _position_z;
			//std::string formattedString = stream.str();
			//const char* cStr = formattedString.c_str();

			//test2
			//int unformattedRequest[5] = { 1, 123, 456, 789, 101112 };
			//std::ostringstream stream;
			//for (int i = 0; i < 5; ++i) {
			//	stream << unformattedRequest[i] << "\n";
			//}
			//std::string formattedString = stream.str();
			//const char* cStr = formattedString.c_str();

			//test3
			//std::ostringstream stream;
			//stream << unformattedRequest[0] << " "
			//	<< std::fixed << std::setprecision(2)
			//	<< unformattedRequest[1] << " "
			//	<< unformattedRequest[2] << " "
			//	<< unformattedRequest[3] << " "
			//	<< unformattedRequest[4];
			//std::string formattedString = stream.str();
			//const char* cStr = formattedString.c_str();

			//testlog
			//ME_LOG_ERROR(cStr);

			memcpy(&formattedRequest, unformattedRequest, sizeof(formattedRequest));
			std::cout << formattedRequest << "\n"; 
			int bytesSent = send(serverSocket, formattedRequest, sizeof(formattedRequest), 0);
			if (bytesSent == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					//fprintf(stderr, "send() failed. (%d)\n", error);  // Chat-GPT: Handle send() errors
				}
			}
		}
		void SetupNetwork::EstablishConnection() {
			if (!SearchForServer()) {
				std::thread serverThread(&Server::InitServer, &server);
				serverThread.detach();
				//SearchForServer();
				bool succes = SearchForServer();
				if (succes)
				{
					SendMessageToServer((int)RequestJoin_Code); 
					//ME_LOG_ERROR("succes to connect to server!");
				}
			}
		}
	};
}