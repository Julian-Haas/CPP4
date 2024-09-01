//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "entitymanager.h"
#include "transformcomponent.h"
#include "say.h"
#include "transformcomponent.h"

namespace me
{
	EntityManager::EntityManager()
		: m_IDCounter(0)
	{

	}

	const std::shared_ptr<Entity>& EntityManager::AddEntity()
	{
		++m_IDCounter;
		auto it = m_Entities.insert(std::make_pair(m_IDCounter, std::make_shared<Entity>(m_ComponentStorer, m_IDCounter)));
		std::shared_ptr<Entity>& entity = it.first->second;
		m_ComponentStorer.AddComponent<TransformComponent>(entity);
		return entity;
	}

	std::weak_ptr<Entity> EntityManager::GetEntity(EntityID id)
	{
		const auto it = m_Entities.find(id);
		if (it != m_Entities.end())
			return it->second;

		return std::weak_ptr<Entity>();
	}

	void EntityManager::RemoveEntity(EntityID entityID)
	{
		if (m_Entities.erase(entityID) != 0)
		{
			m_ComponentStorer.RemoveComponents(entityID);
		}
	}

	void EntityManager::UpdateEntities(float dt)
	{
		m_ComponentStorer.UpdateComponents(dt);
	}

	void EntityManager::SetPositionByID(EntityID playerCubeID, float x, float y, float z)
	{
		std::weak_ptr<Entity> cube = GetEntity(playerCubeID);
		std::shared_ptr<Entity> sharedCube = cube.lock();
		auto transform = sharedCube ? sharedCube->GetComponent<TransformComponent>().lock() : std::shared_ptr<TransformComponent>();
		transform->SetPosition(x, y, z);
	}

	void EntityManager::TranslateByID(EntityID playerCubeID, float x, float y, float z)
	{
		std::weak_ptr<Entity> cube = GetEntity(playerCubeID);
		std::shared_ptr<Entity> sharedCube = cube.lock();
		auto transform = sharedCube ? sharedCube->GetComponent<TransformComponent>().lock() : std::shared_ptr<TransformComponent>();
		transform->Translate(x, y, z);
	}
};