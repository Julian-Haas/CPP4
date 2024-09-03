#include "stdafx.h"
#include "client.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include "say.h"
#include "entitymanager.h"

namespace me
{
	Client::Client(PlayerManager playerManager) : server()
		, playerID(0)
		, _position_x(0)
		, _position_y(0)
		, _position_z(30)
		, m_StartingTime(std::chrono::steady_clock::now())
		, m_playerManager(playerManager)
	{
	}
	Client::~Client()
	{
		if (serverSocket != INVALID_SOCKET)
		{
			char closeConnection[3] = { 0, 0, 0 };
			int bytesSent = send(serverSocket, closeConnection, sizeof(closeConnection), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				std::cerr << "Error sending disconnect message. Error code: " << WSAGetLastError() << std::endl;
			}
			if (closesocket(serverSocket) == SOCKET_ERROR) {
				std::cerr << "Error closing socket. Error code: " << WSAGetLastError() << std::endl;
			}
			serverSocket = INVALID_SOCKET;
		}
	}

	bool Client::ReadData()
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
			int selectResult = select(static_cast<int>(serverSocket + 1), &reads, NULL, NULL, &timeout);
			int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
			memcpy(&receivedMessageInFloat, receivedMessage, sizeof(receivedMessageInFloat));
			if (selectResult == -1)
			{
				std::cout << "select failed\n";
				return false;
			}
			if (selectResult == 0)
			{
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
				case JoinRequestAccepted:
					playerID = receivedMessageInFloat[1];
					break;
				case 2:
					return false;
				case 3:
					m_playerManager.ProcessIncomingPlayerData(receivedMessageInFloat);
					break;
				default:
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	void Client::UltraSchreibePlaayerPositionsdaten()
	{
		m_PlayerData.insert(std::make_pair<int, Position>((int)receivedMessageInFloat[1], Position(1, receivedMessageInFloat[2], receivedMessageInFloat[3], receivedMessageInFloat[4])));
		m_PlayerData[(int)receivedMessageInFloat[1]] = Position(1, receivedMessageInFloat[2], receivedMessageInFloat[3], receivedMessageInFloat[4]);
	}
	bool Client::SearchForServer()
	{
		const char* serverIP = "127.0.0.1";
		const char* serverPort = "8080";
		bool sucess = false;
		WSAData d;
		if (WSAStartup(MAKEWORD(2, 2), &d))
		{
			printf("WinSocket failed to initialize\n");
			return false;
		}
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		addrinfo* serverAddrInfo;
		if (getaddrinfo(serverIP, serverPort, &hints, &serverAddrInfo))
		{
			fprintf(stderr, "getaddrinfo() failed. (%d)\n", WSAGetLastError());
			return false;
		}
		PCHAR addressBuffer = nullptr;
		getnameinfo(serverAddrInfo->ai_addr, (socklen_t)serverAddrInfo->ai_addrlen, addressBuffer, sizeof(addressBuffer), 0, 0, NI_NUMERICHOST);
		serverSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
		if (serverSocket == INVALID_SOCKET)
		{
			fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
			return false;
		}
		u_long mode = 1;
		ioctlsocket(serverSocket, FIONBIO, &mode);
		int connectResult = connect(serverSocket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen);
		if (connectResult == SOCKET_ERROR)
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

				connectResult = select(0, NULL, &writeSet, NULL, &timeout);
				if (connectResult > 0)
				{
					printf("Connected!\n");
					sucess = true;
				}
				else if (connectResult == 0)
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
		freeaddrinfo(serverAddrInfo);
		return sucess;
	}
	void Client::EstablishConnection()
	{
		if (!SearchForServer()) {
			std::thread serverThread(&Server::InitServer, &server);
			serverThread.detach();
		}
	}
	void Client::SendPositionToServer(float x, float y, float z)
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - m_StartingTime;
		if (elapsed.count() >= 0.1) {
			char positionDataFormatted[12];
			std::memcpy(&positionDataFormatted[0], &x, sizeof(x));
			std::memcpy(&positionDataFormatted[4], &y, sizeof(y));
			std::memcpy(&positionDataFormatted[8], &z, sizeof(z));
			m_StartingTime = now;
			int bytesSent = send(serverSocket, positionDataFormatted, sizeof(positionDataFormatted), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
				}
			}
		}
	}
	void Client::UltraDebugFunktionOderSo()
	{
		system("cls");
		std::cout << "ClientReceived: " << std::endl;
		std::cout << "Code: " << receivedMessageInFloat[0] << std::endl;
		std::cout << "Player-ID: " << receivedMessageInFloat[1] << std::endl;
		std::cout << "X: " << receivedMessageInFloat[2] << std::endl;
		std::cout << "Y: " << receivedMessageInFloat[3] << std::endl;
		std::cout << "Z: " << receivedMessageInFloat[4] << std::endl;
	}
}