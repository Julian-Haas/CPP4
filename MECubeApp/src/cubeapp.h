//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "MicroEngine\util.h"
#include "MicroEngine\window.h"
#include "MicroEngine\entitymanager.h"
#include "MicroEngine\renderer.h"
#include "MicroEngine\systemmanager.h"
#include "MicroEngine\setupnetwork.h"
#include <map>
namespace me
{
	class Window;
};

namespace capp
{
	namespace ExitCode
	{
		enum Enum
		{
			Success,
			GraphicInitError,
			PresentError
		};
	};

	class CubeApp
	{
	public:
		CubeApp();
		~CubeApp() = default;
		void InstantiateNewPlayer();
		void UpdatePlayerEntitys();
		ExitCode::Enum Run(HINSTANCE hInst);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(CubeApp);
		//int value is the Player ID 
		std::map<int, Position> m_PlayerData;
		std::map<int, me::Entity*> PlayerTrasforms;
		int playerID;
		int selectedPlayerID;
		float m_MessageData[20];
		void UpdateLogic(float deltaTime);
		void InitEntities();
		me::SetupNetwork network;
		me::EntityID m_ControlledEntityID;
		me::EntityID m_CameraID;
		me::EntityID m_LightID;
		me::EntityManager m_EntityManager;
		me::SystemManager m_SystemManager;
		std::unique_ptr<me::Window> m_Window;
		std::shared_ptr<me::Renderer> m_Renderer;
	};

	struct Position
	{
	public:
		Position()
			: x(0)
			, y(0)
			, z(0)
		{}

		Position(float X, float Y, float Z)
			: x(X)
			, y(Y)
			, z(Z)
		{}

		~Position() = default;
		float x;
		float y;
		float z;
	};
}