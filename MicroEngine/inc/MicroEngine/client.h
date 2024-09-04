#pragma once

#include "me_interface.h"
#include <winsock2.h>
#include "server.h"
#include <chrono>
#include "say.h"
#include "playermanager.h"

namespace me
{
	class Client {
	public:
		ME_API Client(PlayerManager playerManager);
		ME_API ~Client();
		ME_API bool ReadData();
		ME_API void SendPositionToServer(float x, float y, float z);
		ME_API void EstablishConnection();
	private:
		bool SearchForServer();
		std::map<int, Position> m_PlayerData;
		SOCKET serverSocket;
		float receivedMessageInFloat[5];
		char receivedMessage[20];
		Server server;
		int temp = 0;
		std::chrono::steady_clock::time_point m_StartingTime;
		PlayerManager m_playerManager;
	};
}