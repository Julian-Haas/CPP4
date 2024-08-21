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
#include "cubeapp.h"
namespace me {
	enum protocol
	{
		RequestJoin_Code = 101,
		SendPosition_Code = 102,
		SendLogOut_Code = 103
	};
	class SetupNetwork {
	private:
		bool _debugFlag = false;
		SOCKET serverSocket;
		float unformattedRequest[5];
		char formattedRequest[20];
		float receivedMessageInFloat[5];
		char receivedMessage[20];
		enum protocol;
		int _playerID = -1;
		float _position_x = 3;
		float _position_y = 4;
		float _position_z = 5;
		Server server;

	public:
		std::map<int, Position> m_PlayerData;
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
		bool SetupNetwork::ReadData()
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
				int selectResult = select(serverSocket + 1, &reads, NULL, NULL, &timeout);
				int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
				memcpy(&receivedMessageInFloat, receivedMessage, sizeof(receivedMessageInFloat));
				if (selectResult == -1) {
					std::cout << "select failed\n";
					return false;
				}
				if (selectResult == 0) {
					//std::cout << "result 0";
					return false;
				}
				if (selectResult > 0 && FD_ISSET(serverSocket, &reads))
				{
					if (bytesReceived <= 0)
					{
						return false;
					}
					switch ((int)receivedMessageInFloat[0])
					{
					case 1:
						_playerID = receivedMessageInFloat[1];
						break;
					case 2:
						return false;
					case 3:
						UltraDebugFunktionOderSo();
						UltraSchreibePlaayerPositionsdaten();
						std::cout << m_PlayerData[0].z << std::endl;
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
		void UltraDebugFunktionOderSo() {
			//std::cout << "ClientReceived: " << std::endl;
			//std::cout << "Code: " << receivedMessageInFloat[0] << std::endl;
			//std::cout << "Player-ID: " << receivedMessageInFloat[1] << std::endl;
			//std::cout << "X: " << receivedMessageInFloat[2] << std::endl;
			//std::cout << "Y: " << receivedMessageInFloat[3] << std::endl;
			//std::cout << "Z: " << receivedMessageInFloat[4] << std::endl;
		}

		void UltraSchreibePlaayerPositionsdaten() {
			m_PlayerData.insert(std::make_pair<int, Position>((int)receivedMessageInFloat[1], Position(1, receivedMessageInFloat[2], receivedMessageInFloat[3], receivedMessageInFloat[4])));
			m_PlayerData[(int)receivedMessageInFloat[1]] = Position(1, receivedMessageInFloat[2], receivedMessageInFloat[3], receivedMessageInFloat[4]);
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
			unformattedRequest[0] = code;
			unformattedRequest[1] = _playerID;
			unformattedRequest[2] = _position_x;
			unformattedRequest[3] = _position_y;
			unformattedRequest[4] = _position_z;
			memcpy(&formattedRequest, unformattedRequest, sizeof(formattedRequest));
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