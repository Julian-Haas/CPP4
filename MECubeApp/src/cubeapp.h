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

		ExitCode::Enum Run(HINSTANCE hInst);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(CubeApp);

		void UpdateLogic(float deltaTime);
		void InitEntities();

		me::EntityID m_ControlledEntityID;
		me::EntityID m_CameraID;
		me::EntityID m_LightID;
		me::EntityManager m_EntityManager;
		me::SystemManager m_SystemManager;
		std::unique_ptr<me::Window> m_Window;
		std::shared_ptr<me::Renderer> m_Renderer;
	};

}