//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "componentstorer.h"
#include "entity.h"

namespace me
{
	EntityChangedEvent::EntityChangedEvent(const std::shared_ptr<Entity>& entity, ComponentID componentID, EntityChange change)
		: m_ComponentID(componentID)
		, m_EntityID(entity->GetID())
		, m_Change(change)
		, m_Entity(entity)
	{
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	ComponentStorer::~ComponentStorer()
	{
		for (auto& componentsByEntityID : m_ComponentsByID)
		{
			for (auto& component : componentsByEntityID)
			{
				if (component)
				{
					EventBus::GetInstance()->RaiseEvent(
						std::make_shared<EntityChangedEvent>(component->GetOwner(), component->GetID(), EntityChange::ComponentRemoved));
				}
			}
		}
	}

	std::weak_ptr<Component> ComponentStorer::GetComponent(EntityID entityID, ComponentID componentID) const
	{
		if (componentID >= m_ComponentsByID.size())
			return std::weak_ptr<Component>();

		const auto& componentsByEntityID = m_ComponentsByID[componentID];
		if (entityID >= componentsByEntityID.size())
			return std::weak_ptr<Component>();

		return componentsByEntityID[entityID];
	}

	std::vector<std::shared_ptr<Component>> ComponentStorer::GetComponents(EntityID entityID) const
	{
		std::vector<std::shared_ptr<Component>> result;

		for (const auto& componentsByEntityID : m_ComponentsByID)
		{
			if (entityID >= componentsByEntityID.size())
				continue;
			auto component = componentsByEntityID[entityID];
			if (component)
				result.push_back(component);
		}
		return result;
	}

	const std::shared_ptr<Component>& ComponentStorer::AddComponent(const std::shared_ptr<Entity>& entity, Component* component)
	{
		ME_ASSERT(component);

        const EntityID entityID = entity->GetID();

		if (component->GetID() >= m_ComponentsByID.size())
			m_ComponentsByID.resize(component->GetID() + 1);

		auto& componentsByEntityID = m_ComponentsByID[component->GetID()];
		if (entityID >= componentsByEntityID.size())
			componentsByEntityID.resize(entityID + 1);

		if (componentsByEntityID[entityID])
		{
			ME_LOG_WARNING("Component already exists, replacing it");
		}

		componentsByEntityID[entityID] = std::shared_ptr<Component>(component);
		component->SetOwner(entity);
		component->Init();

		EventBus::GetInstance()->RaiseEvent(
			std::make_shared<EntityChangedEvent>(component->GetOwner(), component->GetID(), EntityChange::ComponentAdded));

		return componentsByEntityID[entityID];
	}

	void ComponentStorer::RemoveComponent(EntityID entityID, ComponentID componentID)
	{
		if (componentID >= m_ComponentsByID.size())
			return;

		auto& componentsByEntityID = m_ComponentsByID[componentID];

		if (entityID >= componentsByEntityID.size())
			return;
		const auto& entity = componentsByEntityID[entityID]->GetOwner();
		if (componentsByEntityID[entityID])
		{
			componentsByEntityID[entityID].reset();
			EventBus::GetInstance()->RaiseEvent(
				std::make_shared<EntityChangedEvent>(entity, componentID, EntityChange::ComponentRemoved));
		}
	}

	void ComponentStorer::RemoveComponents(EntityID entityID)
	{
		for(ComponentID i = 0; i < m_ComponentsByID.size(); ++i)
		{
			RemoveComponent(entityID, i);
		}
	}

	void ComponentStorer::UpdateComponents(float deltaTime)
	{
		for (auto& componentsByEntityID : m_ComponentsByID)
		{
			for (auto& component : componentsByEntityID)
			{
				if (component && component->IsEnabled())
					component->Update(deltaTime);
			}
		}
	}
}