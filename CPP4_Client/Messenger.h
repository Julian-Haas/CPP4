#pragma once
#include <WinSock2.h>
#include <iostream>
#include <string>

class Messenger
{
private:
	SOCKET serverSocket;
	//std::string serverRequest;
	//std::string nameOfActiveUser = ".";
	int unformattedRequest[5];
	char formattedRequest[20];
	int receivedMessageInInt[5];
	char receivedMessage[20];
	enum protocol;
	bool WaitForServerResponse();
	void OpenMainMenu();
	void ClearInputBuffer();
	void Play();
	void SendToServer();
	void RequestJoin();
	void SendPosition();
	void SendLogOut();
	void AddPlayerID();
	void AddPositionToRequest();
	void SetProtocolCode(int code);
	//void ExtendRequest(std::string appendedParameter);
	//void SetRequestCode(int requestCode);
	//void DisplayReceivedHistory();
	//void PostAMessage(std::string message);
	//bool RegisterOnServer(std::string Username, std::string Password);
	//void AddMessageLenght(std::string msg);
public:
	void StartMessenger(int argc, char* argv[]);
};