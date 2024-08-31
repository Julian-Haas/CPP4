#pragma once
#include "me_interface.h"
#include <winsock2.h>
#include "MEServer.h"

namespace me {
	class Client {
	private:
		bool _debugFlag = false;
		SOCKET serverSocket;
		float unformattedRequest[5];
		char formattedRequest[20];
		float receivedMessageInFloat[5];
		char receivedMessage[20];
		float playerID;
		float _position_x;
		float _position_y;
		float _position_z;
		Server server;

	public:
		std::map<int, Position> m_PlayerData;
		ME_API Client();
		ME_API ~Client() = default;
		ME_API bool ReadData();
		ME_API void UltraDebugFunktionOderSo();
		ME_API void UltraSchreibePlaayerPositionsdaten();
		ME_API bool SearchForServer();
		ME_API void SendMessageToServer(float code);
		ME_API void EstablishConnection();
	};
}