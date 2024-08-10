//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "component.h"
#include "util.h"

namespace me
{
	class ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;
		virtual void Update(float deltaTime) = 0;

		//Return type outer vector is OR, inner vector is AND
		virtual const std::vector<std::vector<ComponentID>>& GetRequiredComponents() const = 0;

	protected:
		friend class SystemManager;
		virtual void OnAddEntity(const std::shared_ptr<Entity>& entity) = 0;
		virtual void OnRemoveEntity(EntityID entityID) = 0;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(ISystem);
	};
}