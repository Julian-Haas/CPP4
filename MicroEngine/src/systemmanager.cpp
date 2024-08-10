//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "systemmanager.h"
#include "eventbus.h"
#include "componentstorer.h"
#include "entity.h"

namespace me
{
	SystemManager::SystemManager()
	{
		EventBus::GetInstance()->Register(EntityChangedEvent::ID, reinterpret_cast<size_t>(this), std::bind(&SystemManager::OnEntityChanged, this, std::placeholders::_1));
	}

	SystemManager::~SystemManager()
	{
		EventBus::GetInstance()->Unregister(EntityChangedEvent::ID, reinterpret_cast<size_t>(this));
	}

	void SystemManager::AddSystem(const std::shared_ptr<ISystem>& system)
	{
		m_Systems.push_back(system);
	}

	void SystemManager::Update(float deltaTime)
	{
		for (auto& system : m_Systems)
		{
			system->Update(deltaTime);
		}
	}

	bool AreConditionsSatisfied(const Entity& entity, const std::vector<std::vector<ComponentID>>& orConditions)
	{
		for (const auto& andConditions : orConditions)
		{
			bool hasRequiredComponents = true;
			for (ComponentID componentID : andConditions)
			{
				if (entity.GetComponent(componentID).expired())
				{
					hasRequiredComponents = false;
					break;
				}
			}

			if (hasRequiredComponents)
				return true;
		}
		
		return false;
	}

	void SystemManager::OnEntityChanged(const std::shared_ptr<IEvent>& event)
	{
		auto ev = std::static_pointer_cast<EntityChangedEvent>(event);

		switch (ev->m_Change)
		{
		case EntityChange::ComponentAdded:
		{
            const std::shared_ptr<Entity> entity = ev->GetEntity().lock();
			if (!entity)
				return;
			for (auto& system : m_Systems)
			{
				if(AreConditionsSatisfied(*entity, system->GetRequiredComponents()))
					system->OnAddEntity(entity);
			}
			break;
		}
		case EntityChange::ComponentRemoved:
			for (auto& system : m_Systems)
			{
				std::shared_ptr<Entity> entity = ev->GetEntity().lock();
				if(!AreConditionsSatisfied(*entity, system->GetRequiredComponents()))
					system->OnRemoveEntity(ev->m_EntityID);
			}
			break;
		}
	}

}