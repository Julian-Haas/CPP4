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
	Client::Client(PlayerManager playerManager) : m_Server()
		, m_StartingTime(std::chrono::steady_clock::now())
		, m_playerManager(playerManager)
		, m_ServerSocket(INVALID_SOCKET)
		, m_ReceivedMessage{}
		, m_MessageBuffer{}
	{
	}
	Client::~Client()
	{
		if (m_ServerSocket != INVALID_SOCKET)
		{
			char closeConnection[3] = { 0, 0, 0 };
			int bytesSent = send(m_ServerSocket, closeConnection, sizeof(closeConnection), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				std::cerr << "Error sending disconnect message. Error code: " << WSAGetLastError() << std::endl;
			}
			if (closesocket(m_ServerSocket) == SOCKET_ERROR) {
				std::cerr << "Error closing socket. Error code: " << WSAGetLastError() << std::endl;
			}
			m_ServerSocket = INVALID_SOCKET;
		}
	}
	bool Client::ReadData()
	{
		bool readAllData = false;
		while (!readAllData)
		{
			fd_set reads;
			FD_ZERO(&reads);
			FD_SET(m_ServerSocket, &reads);
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 50000;
			int selectResult = select(static_cast<int>(m_ServerSocket + 1), &reads, NULL, NULL, &timeout);
			int bytesReceived = recv(m_ServerSocket, m_MessageBuffer, sizeof(m_MessageBuffer), 0);
			memcpy(&m_ReceivedMessage, m_MessageBuffer, sizeof(m_ReceivedMessage));
			if (selectResult == -1)
			{
				std::cout << "select failed\n";
				return false;
			}
			if (selectResult == 0)
			{
				return false;
			}
			if (selectResult > 0 && FD_ISSET(m_ServerSocket, &reads))
			{
				if (bytesReceived <= 0)
				{
					return false;
				}
				switch ((int)m_ReceivedMessage[0])
				{
				case JoinRequestAccepted:
					break;
				case 2:
					return false;
				case 3:
					m_playerManager.ProcessIncomingPlayerData(m_ReceivedMessage);
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
		m_ServerSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
		if (m_ServerSocket == INVALID_SOCKET)
		{
			fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
			return false;
		}
		u_long mode = 1;
		ioctlsocket(m_ServerSocket, FIONBIO, &mode);
		int connectResult = connect(m_ServerSocket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen);
		if (connectResult == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				fd_set writeSet;
				FD_ZERO(&writeSet);
				FD_SET(m_ServerSocket, &writeSet);

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
		if (!SearchForServer())
		{
			std::thread serverThread(&Server::InitServer, &m_Server);
			serverThread.detach();
		}
	}
	void Client::SendPositionToServer(float x, float y, float z)
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - m_StartingTime;
		if (elapsed.count() >= 0.1)
		{
			char positionDataFormatted[13] = {};
			positionDataFormatted[0] = SendPosition;
			std::memcpy(&positionDataFormatted[1], &x, sizeof(x));
			std::memcpy(&positionDataFormatted[5], &y, sizeof(y));
			std::memcpy(&positionDataFormatted[9], &z, sizeof(z));
			m_StartingTime = now;
			int bytesSent = send(m_ServerSocket, positionDataFormatted, sizeof(positionDataFormatted), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
				}
			}
		}
	}
}