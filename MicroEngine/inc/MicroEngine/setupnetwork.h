#pragma once
#include "me_interface.h"
#include <winsock2.h>
#include "MEServer.h"

namespace me {
	enum protocol {
		RequestJoin_Code = 101,
		SendPosition_Code = 102,
		SendLogOut_Code = 103
	};

	struct Position
	{
	public:
		SOCKET playersocket;
		float x;
		float y;
		float z;
		Position() = default;
		Position(SOCKET socket, float xPos, float yPos, float zPos)
			: playersocket(socket)
			, x(xPos)
			, y(yPos)
			, z(zPos)
		{}

		~Position()
		{}
	};

	class SetupNetwork {
	private:
		bool _debugFlag = false;
		SOCKET serverSocket;
		float unformattedRequest[5];
		char formattedRequest[20];
		float receivedMessageInFloat[5];
		char receivedMessage[20];
		float _playerID = -1;
		float _position_x = 3;
		float _position_y = 4;
		float _position_z = 5;
		Server server;
	public:
		std::map<int, Position> m_PlayerData;
		ME_API void Testfunktion();
		ME_API SetupNetwork();
		ME_API ~SetupNetwork() = default;
		ME_API void UpdateTheServer();
		ME_API bool ReadData();
		ME_API void UltraDebugFunktionOderSo();
		ME_API void UltraSchreibePlaayerPositionsdaten();
		ME_API bool SearchForServer();
		ME_API void SendMessageToServer(float code);
		ME_API void EstablishConnection();
	};
}