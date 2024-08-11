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

Server::Server(std::vector<std::vector<std::string>> nutzer)
	: user(nutzer)
{
}
enum Server::protocol
{
	CheckUsernameForExistance_Client = 1,
	CheckPasswordForCorrectness_Client = 2,
	DisplayHistoryOfUser_Client = 3,
	PostAMessage_Client = 4,
	RegisterUser_Client = 5,
	CheckUsernameForExistance_Server = 101,
	CheckPasswordForCorrectness_Server = 102,
	DisplayHistoryOfUser_Server = 103,
	PostAMessage_Server = 104,
	RegisterUser_Server = 105,
	InvalidUserName_Server = 106
};

std::string Server::AddMessageLength(std::string msg, int length)
{
	// L�nge in zwei Bytes aufteile
	unsigned char val1 = (length >> 8) & 0xFF;
	unsigned char val2 = length & 0xFF;

	msg.insert(msg.begin(), val2);
	msg.insert(msg.begin(), val1);

	return msg;
}


int Server::GetStringLenght(int start)
{
	int length = (request[start] << 8) | request[start + 1];
	return length;
}

void Server::SendToClient(SOCKET i, std::string msg)
{
	char formatedAnswer[4096];
	memcpy(formatedAnswer, msg.data(), msg.size());
	formatedAnswer[msg.size()] = '\0';
	send(i, formatedAnswer, msg.size() + 1, 0); // �bergeben Sie die tats�chlich kopierte Datenl�nge
}

std::string Server::GetPassword(int start, int lenght)
{
	std::string password = "";
	for (int i = start; i < (start + lenght); i++)
	{
		password += request[i];
	}
	return password;
}

std::string Server::ExtractTweet()
{

	int NameLenght = GetStringLenght(1);
	int tweetLenghtPos = NameLenght + 3;
	int tweetLenght = GetStringLenght(tweetLenghtPos);
	std::string tweet = std::string(request + NameLenght + 5, tweetLenght);
	return tweet;
}

int Server::CheckForUserName()
{
	int nameLength = GetStringLenght(1);
	activeUser = std::string(request + 3, nameLength);
	if (activeUser == "")
	{
		return 3;
	}
	for (int i = 0; i < user.size(); i++)
	{
		userTweetID = i;
		if (activeUser == user[i][0])
		{
			userIndex = i;
			return 1;
		}
	}
	return 2;
}

std::string Server::GetUserPosts()
{
	int usernameLenght = GetStringLenght(1);
	std::string username = std::string(request + 3, usernameLenght);
	std::string answer;
	char msgCode = char(DisplayHistoryOfUser_Server);
	answer += msgCode;

	for (int i = 0; i <= user.size() - 1; i++)
	{
		if (username == user[i][0])
		{
			userTweetID = i;
			break;
		}
	}
	if (userTweetID >= tweets.size())
	{
		answer += char(0);
		answer = AddMessageLength(answer, usernameLenght);
		answer.append(username);
		return answer; //early out if there are no posts 
	}
	//Safe Posts
	int endOfLoop = tweets[userTweetID].size() - 10;
	int postAmount;
	if (endOfLoop <= 0)
	{
		endOfLoop = 0;
		postAmount = tweets[userTweetID].size();
	}
	else
	{
		postAmount = 10;
	}
	char postAmountChar = postAmount;
	answer += postAmountChar;
	answer = AddMessageLength(answer, usernameLenght);
	answer.append(username);
	for (int i = tweets[userTweetID].size() - 1; i >= endOfLoop; i--)
	{
		int time = postTimes[userTweetID][i];
		char bytes[4];
		bytes[0] = (time >> 24) & 0xFF;
		bytes[1] = (time >> 16) & 0xFF;
		bytes[2] = (time >> 8) & 0xFF;
		bytes[3] = time & 0xFF;
		answer += bytes[0];
		answer += bytes[1];
		answer += bytes[2];
		answer += bytes[3];
		std::string temp = tweets[userTweetID][i];
		int lenghtOfPost = (tweets[userTweetID][i].length());
		if (lenghtOfPost > 0)
		{
			answer = AddMessageLength(answer, lenghtOfPost);
			answer.append(tweets[userTweetID][i]);
		}
	}
	return answer;
}

void Server::DisplayUserHistory(SOCKET i)
{
	std::string msg = GetUserPosts();

	SendToClient(i, msg);
}


void Server::CheckUserNameForExistance(SOCKET i)
{
	int returnCode = CheckForUserName();
	std::string answer;
	if (returnCode != 3) {

		answer = char(CheckUsernameForExistance_Server);
		char exitCode = char(returnCode);
		answer += exitCode;
		SendToClient(i, answer);
	}
	else {
		answer = char(InvalidUserName_Server);
		char exitCode = char(returnCode);
		answer += exitCode;
		SendToClient(i, answer);
	}
	return;
}
void Server::CheckPasswordForCorrectness(SOCKET i)
{
	std::string password;
	std::string answer;
	int passwortStart = GetStringLenght(1) + 5;
	int passwortLenght = GetStringLenght(passwortStart - 2);
	password = GetPassword(passwortStart, passwortLenght);
	answer = "";
	if (user[userIndex][1] == password)
	{
		answer = char(CheckPasswordForCorrectness_Server);
		char returnCode = char(1);
		answer += returnCode;
		SendToClient(i, answer);
	}
	else
	{
		answer = char(CheckPasswordForCorrectness_Server);
		char returnCode = char(2);
		answer += returnCode;
		SendToClient(i, answer);
	}
}
int Server::GetUserTweetID()
{
	std::string answer;
	int usernameLenght = GetStringLenght(1);
	std::string username = std::string(request + 3, usernameLenght);
	for (int i = 0; i <= user.size() - 1; i++)
	{
		if (username == user[i][0])
		{
			return i;
		}
	}
	return -1;
}
int Server::PostUserMessage(SOCKET i)
{
	std::string answer;
	//when a message gets psoted, store seconds_since_epoch somewhere with the post
	const auto p1 = std::chrono::system_clock::now();
	std::time_t post_time = std::chrono::system_clock::to_time_t(p1);
	int seconds_since_epoch = static_cast<int>(post_time);
	int userTweetID = GetUserTweetID();
	if (userTweetID == -1)
	{
		answer.clear();
		answer += PostAMessage_Server;
		char exitCode = 2;
		answer += exitCode;
		SendToClient(i, answer);
		return-1;
	}
	answer = ExtractTweet();

	if (userTweetID >= tweets.size()) {
		tweets.resize(userTweetID + 4);
	}
	if (userTweetID >= postTimes.size()) {
		postTimes.resize(userTweetID + 4);
	}
	tweets[userTweetID].push_back(answer);
	postTimes[userTweetID].push_back(seconds_since_epoch);
	tweetCounter++;
	answer.clear();
	answer += PostAMessage_Server;
	char exitCode = 2;
	answer += exitCode;
	SendToClient(i, answer);
	return 0;
}
void Server::FinishRegistration(SOCKET i)
{
	user[index][0] = activeUser;
	std::string password;
	std::string answer;
	int passwortStart = GetStringLenght(1);
	passwortStart += 5;
	int passwortLenght = GetStringLenght(passwortStart - 2);
	password = GetPassword(passwortStart, passwortLenght);
	user[index][1] = password;
	answer = "";
	answer += RegisterUser_Server;
	answer += 1;
	SendToClient(i, answer);
	index++;
	userIndex = index;
}
void Server::HandleIncomingRequest(bool& readingRequest, SOCKET i) {

	int postWasSuccesFull;
	switch (request[0]) {
	case 1:
		printf("checking, if username exists...\n");
		CheckUserNameForExistance(i);
		readingRequest = false;
		break;
	case 2:
		printf("checking, if Password is correct...\n");
		CheckPasswordForCorrectness(i);
		readingRequest = false;
		break;
	case 3:
		printf("Displaying user history...\n");
		DisplayUserHistory(i);
		break;
	case 4:
		printf("posting message...\n");
		postWasSuccesFull = PostUserMessage(i);
		if (postWasSuccesFull == -1)
		{
			std::cout << "Failed to post the Message:\n Error code: -1! Username not found!\n";
		}
		readingRequest = false;
		break;
	case 5:
		printf("finish registration process...\n");
		FinishRegistration(i);
		readingRequest = false;
		break;
	default:
		printf("unhandled request");
		readingRequest = false;
		break;
	}
}

int Server::InitServer(int argc, char* argv[])
{
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