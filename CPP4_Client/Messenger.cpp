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
	while (true) {
		std::cout << "Press 1 to log in." << std::endl;
		WaitForServerResponse();
	}
}

void Messenger::Play() {
	std::cout << "Press 2 to send your position to the Server. Please only do this while being logged in." << std::endl;
	std::cout << "Press 3 to log out. Please only do this while being logged in." << std::endl;
}

void Messenger::SendToServer()
{
	memcpy(formattedRequest, unformattedRequest, sizeof(unformattedRequest));
	send(serverSocket, formattedRequest, sizeof(formattedRequest), 0);
}

void Messenger::RequestJoin() {
	SetProtocolCode(RequestJoin_Code);
	AddPositionToRequest();
	SendToServer();
}

void Messenger::SendPosition() {
	SetProtocolCode(SendPosition_Code);
	AddPositionToRequest();
	SendToServer();
}

void Messenger::SendLogOut() {
	SetProtocolCode(SendLogOut_Code);
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
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	addrinfo* server;
	if (getaddrinfo(argv[1], argv[2], &hints, &server))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", WSAGetLastError());
		return;
	}
	printf("Remote adress is :\n");
	char adressBuffer[100];
	getnameinfo(server->ai_addr, server->ai_addrlen, adressBuffer, sizeof(adressBuffer), 0, 0, NI_NUMERICHOST);
	printf("%s\n", adressBuffer);
	printf("Creating socket...\n");
	serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
	if (serverSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "socket(9 failed. %d)\n", WSAGetLastError());
		return;
	}
	printf("Connection to server...\n");
	if (connect(serverSocket, server->ai_addr, server->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", WSAGetLastError());
		return;
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
		timeout.tv_usec = 100000;
		if (select(serverSocket + 1, &reads, 0, 0, &timeout) < 0)
		{
			fprintf(stderr, "select() failed. (%d)\n", WSAGetLastError());
			return false;
		}

		if (FD_ISSET(serverSocket, &reads))
		{
			int bytesReceived = recv(serverSocket, receivedMessage, sizeof(receivedMessage), 0);
			int val = 0;
			memcpy(receivedMessage, receivedMessageInInt, sizeof(receivedMessageInInt));
			switch (receivedMessageInInt[0])
			{
			case 1:
				std::printf("You are now logged in and your player-id is %d\n", receivedMessageInInt[1]);
				Play();
				//val = receivedMessage[1];
				//return (val == 1);
				break;
			case 2:
				std::printf("Login was not successful.");
				//val = receivedMessage[1];
				//return (val == 1);
				break;
			case 3:
				std::printf("Player %d is at position %d / %d / %d \n", receivedMessageInInt[1], receivedMessageInInt[2], receivedMessageInInt[3], receivedMessageInInt[4]);
				//return true;
				break;
			default:
				break;
			}
			if (bytesReceived < 1)
			{
				printf("Connection closed.\n");
				return false;
			}
		}
	}
}



//void Messenger::AddMessageLenght(std::string msg)
//{
//	int length = msg.length();
//
//	char val1;
//	char val2;
//	if (length > 255)
//	{
//		int rest = length - 255;
//		val1 = char(255);
//		val2 = char(rest);
//		serverRequest += val1;
//		serverRequest += val2;
//	}
//	else
//	{
//		val1 = char(length);
//		val2 = char(0);
//		serverRequest += val1;
//		serverRequest += val2;
//	}
//}
//
//int GetStringLenght(char request[], int start)
//{
//	return request[start] + request[start + 1];
//}
//
//void Messenger::SetRequestCode(int requestCode)
//{
//	serverRequest = char(requestCode);
//}
//
//void Messenger::ExtendRequest(std::string appendedParameter)
//{
//	AddMessageLenght(appendedParameter);
//	serverRequest.append(appendedParameter);
//}

//RequestJoin{
//	SetRequestCode(RequestJoin_Code);
//	AddPositionToRequest();
//	SendToServer();
//}
//
//SendPosition{
//	SetRequestCode(SendPosition_Code);
//	AddPositionToRequest();
//	SendToServer();
//}
//
//SendLogOut{
//	SetRequestCode(SendLogOut_Code);
//	AddPositionToRequest();
//	SendToServer();
//}
//
//AddPositionToRequest{
//	ExtendRequest(_position_x);
//	ExtendRequest(_position_y);
//	ExtendRequest(_position_z);
//}