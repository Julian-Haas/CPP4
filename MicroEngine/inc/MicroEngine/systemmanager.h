//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include <memory>

#include "system.h"
#include "me_interface.h"

namespace me
{
	class IEvent;

	class SystemManager
	{
	public:
		ME_API SystemManager();
		ME_API ~SystemManager();

		ME_API void AddSystem(const std::shared_ptr<ISystem>& system);

		ME_API void Update(float deltaTime);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(SystemManager);
		void OnEntityChanged(const std::shared_ptr<IEvent>& event);

		std::vector<std::shared_ptr<ISystem>> m_Systems;
	};
}