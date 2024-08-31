#pragma once
#include "me_interface.h"
#include <winsock2.h>
#include "MEServer.h"
#include <chrono>
#include "say.h"

namespace me {
	class Client {
	private:
		bool _debugFlag = false;
		SOCKET serverSocket;
		float receivedMessageInFloat[5];
		char receivedMessage[20];
		float playerID;
		float _position_x;
		float _position_y;
		float _position_z;
		Server server;
		//Helper helper;
		int temp = 0;
		std::chrono::steady_clock::time_point m_StartingTime;

	public:
		std::map<int, Position> m_PlayerData;
		ME_API Client();
		ME_API ~Client() = default;
		ME_API bool ReadData();
		ME_API void UltraDebugFunktionOderSo();
		ME_API void UltraSchreibePlaayerPositionsdaten();
		ME_API bool SearchForServer();
		ME_API void SendPositionToServer(float x, float y, float z);
		ME_API void EstablishConnection();
	};
}