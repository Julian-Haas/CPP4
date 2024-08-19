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
		int unformattedRequest[5];
		char formattedRequest[20];
		int receivedMessageInInt[5];
		char receivedMessage[20];
		enum protocol;
		int _position_x = 3;
		int _position_y = 4;
		int _position_z = 5;
		int _playerID = -1;
		Server server;
	public:
		SetupNetwork()
			: server()
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
				timeout.tv_usec = 100000;
				int selectResult = select((int)(serverSocket + 1), &reads, 0, 0, &timeout);
				if (FD_ISSET(serverSocket, &reads))
				{
					int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
					if (bytesReceived == 0)
					{
						readAllData = true;
						return false;
					}
					memcpy(receivedMessageInInt, receivedMessage, sizeof(receivedMessage));
					switch (receivedMessageInInt[0])
					{
					case 1:
						_playerID = receivedMessageInInt[1];
						break;
					case 2:
						//elaborate
						return false;
						break;
					case 3:
						//update player pos
						break;
					default:
						break;
					}
				}
			}
			return true;
		}
		bool SetupNetwork::SearchForServer()
		{
			//magic numbers
			const char* serverIP = "192.168.178.28";
			const char* serverPort = "5000";
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
					timeout.tv_sec = 5;
					timeout.tv_usec = 0;

					result = select(0, NULL, &writeSet, NULL, &timeout);
					if (result > 0)
					{
						printf("Connected!\n");
					}
					else if (result == 0)
					{
						printf("Connection timed out.\n");
						return false;
					}
					else
					{
						fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
						return false;
					}
				}
				else
				{
					fprintf(stderr, "connect() failed. (%d)\n", error);
					return false;
				}
			}
			freeaddrinfo(server);
			printf("Connected!\n");
			return true;
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
			memcpy(formattedRequest, unformattedRequest, sizeof(unformattedRequest));
			int bytesSent = send(serverSocket, formattedRequest, sizeof(formattedRequest), 0);
			if (bytesSent == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					fprintf(stderr, "send() failed. (%d)\n", error);  // Chat-GPT: Handle send() errors
				}
			}
		}
		void SetupNetwork::EstablishConnection() {
			if (!SearchForServer()) { 
				std::thread serverThread(&Server::InitServer, &server);
				serverThread.detach(); 
				bool succes = SearchForServer();
				//if(succes)
				//{
				//	ME_LOG_ERROR("succes to connect to server!"); 
				//	//BeepBeep(); 
				//}
			}	//
		}
	};
}