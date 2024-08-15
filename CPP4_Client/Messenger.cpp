#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "iphlpapi.lib")

#include <cstdio>
#include <conio.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <sstream>
#include <iomanip>

#include "Messenger.h"

int _position_x = 3;
int _position_y = 4;
int _position_z = 5;

int _playerID = -1;

enum Messenger::protocol
{
	RequestJoin_Code = 101,
	SendPosition_Code = 102,
	SendLogOut_Code = 103
};

void Messenger::OpenMainMenu() {
	char ch;
	while (true) {
		std::cout << "Press 1 to log in." << std::endl;
		ClearInputBuffer();
		ch = _getch();
		if (ch == '1') {
			RequestJoin();
			for (int i = 0; i <= 4; ++i) {
				// Use printf to display each character
				printf("%d", unformattedRequest[i]);
				printf("\n");
			}

			WaitForServerResponse();
			break;
		}
	}
}

void Messenger::ClearInputBuffer() {
	while (_kbhit()) {
		_getch();
	}
}

void Messenger::Play() {
	std::cout << "Press 2 to send your position to the Server. Please only do this while being logged in." << std::endl;
	std::cout << "Press 3 to log out. Please only do this while being logged in." << std::endl;
}

void Messenger::SendToServer()
{
	memcpy(formattedRequest, unformattedRequest, sizeof(unformattedRequest));
	int bytesSent = send(serverSocket, formattedRequest, sizeof(formattedRequest), 0);
	if (bytesSent == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK) {
			fprintf(stderr, "send() failed. (%d)\n", error);  // Chat-GPT: Handle send() errors
		}
	}
}

void Messenger::RequestJoin() {
	SetProtocolCode(RequestJoin_Code);
	AddPlayerID();
	AddPositionToRequest();
	SendToServer();
}

void Messenger::SendPosition() {
	SetProtocolCode(SendPosition_Code);
	AddPlayerID();
	AddPositionToRequest();
	SendToServer();
}

void Messenger::SendLogOut() {
	SetProtocolCode(SendLogOut_Code);
	AddPlayerID();
	AddPositionToRequest();
	SendToServer();
}

void Messenger::AddPlayerID() {
	unformattedRequest[1] = _playerID;
}

void Messenger::AddPositionToRequest() {
	unformattedRequest[2] = _position_x;
	unformattedRequest[3] = _position_y;
	unformattedRequest[4] = _position_z;
}

void Messenger::SetProtocolCode(int code) {
	unformattedRequest[0] = code;
}

void Messenger::StartMessenger(int argc, char* argv[])
{
	WSAData d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("WinSocket failed to initialize\n");
		return;
	}
	if (argc < 3)
	{
		printf("usage tcp client [ip] [port]\n");
		return;
	}
	printf("Configuring remote address\n");
	sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(5000);
	server.sin_addr.s_addr = INADDR_ANY;//inet_addr("178.203.204.116");

	// Hardcoded IP and port

	const char* serverIP = "178.203.204.116";  // Chat-GPT: Use the public IP address
	//const char* serverIP = "178.203.204.116";  // Chat-GPT: Use the public IP address
	const char* serverPort = "5000";           // Chat-GPT: Use the port 5000

	printf("Creating socket...\n");
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket() failed. (%d)\n", WSAGetLastError());
		return;
	}
	u_long mode = 1; // Set non-blocking mode
	ioctlsocket(serverSocket, FIONBIO, &mode);  // Chat-GPT: Set the socket to non-blocking mode
	printf("Connecting to server...\n");
	int result = connect(serverSocket, );
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
		{
			// Use select() to wait for the connection to complete
			fd_set writeSet;
			FD_ZERO(&writeSet);
			FD_SET(serverSocket, &writeSet);

			timeval timeout;
			timeout.tv_sec = 5;  // Chat-GPT: Timeout after 5 seconds
			timeout.tv_usec = 0;

			result = select(0, NULL, &writeSet, NULL, &timeout);
			if (result > 0)
			{
				printf("Connected!\n");
			}
			else if (result == 0)
			{
				printf("Connection timed out.\n");
				return;
			}
			else
			{
				fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
				return;
			}
		}
		else
		{
			fprintf(stderr, "connect() failed. (%d)\n", error);
			return;
		}
	}
	freeaddrinfo(server);
	printf("Connected!\n");
	printf("To send data, enter the text followed by enter \n");
	OpenMainMenu();
}

bool Messenger::WaitForServerResponse()
{
	while (true)
	{
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(serverSocket, &reads);
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000; // Chat-GPT: 100 milliseconds

		int selectResult = select(serverSocket + 1, &reads, 0, 0, &timeout);
		if (selectResult < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
			return false;
		}
		else if (selectResult == 0)
		{
			// Timeout, no action needed, loop continues
			continue;
		}

		if (FD_ISSET(serverSocket, &reads))
		{
			int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
			if (bytesReceived == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK)
				{
					fprintf(stderr, "recv() failed. (%d)\n", error);  // Chat-GPT: Handle recv() errors
				}
				return false;
			}
			else if (bytesReceived == 0)
			{
				printf("Connection closed.\n");
				return false;
			}
			else
			{
				// Process received data
				int val = 0;
				memcpy(receivedMessageInInt, receivedMessage, sizeof(receivedMessage));
				std::cout << std::endl;
				for (int number : receivedMessageInInt) {
					std::cout << number << std::endl;
				}
				_getch();
				switch (receivedMessageInInt[0])
				{
				case 1:
					std::printf("You are now logged in and your player-id is %d\n", receivedMessageInInt[1]);
					_playerID = receivedMessageInInt[1];
					Play();
					break;
				case 2:
					std::printf("Login was not successful.");
					break;
				case 3:
					std::printf("Player %d is at position %d / %d / %d \n", receivedMessageInInt[1], receivedMessageInInt[2], receivedMessageInInt[3], receivedMessageInInt[4]);
					break;
				default:
					break;
				}
			}
		}
	}
}
