//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <vector>
#include <memory>
#include "util.h"
#include "entity.h"
#include "component.h"
#include "componentstorer.h"
#include "me_interface.h"

namespace me
{
	class EntityManager
	{
	public:
		ME_API EntityManager();
		ME_API ~EntityManager() = default;

		ME_API const std::shared_ptr<Entity>& AddEntity();
		ME_API std::weak_ptr<Entity> GetEntity(EntityID id);
		ME_API void RemoveEntity(EntityID id);
		ME_API void UpdateEntities(float dt);

		template <class T>
		std::shared_ptr<T> AddComponent(EntityID entityID)
		{
            const auto entity = GetEntity(entityID).lock();
			if (!entity)
				return std::shared_ptr<T>();
			return m_ComponentStorer.AddComponent<T>(entity);
		}
		void RemoveComponent(EntityID entityID, ComponentID componentID) { m_ComponentStorer.RemoveComponent(entityID, componentID); }

	    template <class T>
		std::weak_ptr<T> GetComponent(EntityID entityID) const { return std::static_pointer_cast<T>(GetComponent(entityID, T::ID).lock()); }
		std::weak_ptr<Component> GetComponent(EntityID entityID, ComponentID componentID) const { return m_ComponentStorer.GetComponent(entityID, componentID); }
		std::vector<std::shared_ptr<Component>> GetComponents(EntityID entityID) const { return m_ComponentStorer.GetComponents(entityID); }
	private:
		ME_MOVE_COPY_NOT_ALLOWED(EntityManager);

		std::map<EntityID, std::shared_ptr<Entity>> m_Entities;
		EntityID m_IDCounter;
		ComponentStorer m_ComponentStorer;
	};

};		//map is a sorted container, iteration will be in component id order
