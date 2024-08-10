//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "util.h"
#include "component.h"
#include "eventbus.h"
#include "me_interface.h"

namespace me
{
	enum class EntityChange
	{
		ComponentAdded,
		ComponentRemoved,
	};

	class EntityChangedEvent : public IEvent
	{
	public:
		ME_API EntityChangedEvent(const std::shared_ptr<Entity>& entity, ComponentID componentID, EntityChange change);
		virtual ME_API ~EntityChangedEvent() = default;
		static const EventID ID = EngineEventID::EntityChanged;
		virtual EventID GetID() const override { return ID; }
		std::weak_ptr<Entity>& GetEntity() { return m_Entity; }
		const ComponentID m_ComponentID;
		const EntityID m_EntityID;
		const EntityChange m_Change;
	private:
		ME_MOVE_COPY_NOT_ALLOWED(EntityChangedEvent);
		std::weak_ptr<Entity> m_Entity;
	};

	//TODO: use a custom allocator to store components in contiguous memory
	class ComponentStorer
	{
	public:
		ME_API ComponentStorer() = default;
		ME_API ~ComponentStorer();

		ME_API std::weak_ptr<Component> GetComponent(EntityID entityID, ComponentID componentID) const;
		ME_API std::vector<std::shared_ptr<Component>> GetComponents(EntityID entityID) const;
		ME_API const std::shared_ptr<Component>& AddComponent(const std::shared_ptr<Entity>& entity, Component* component);
		template <class T>
		std::shared_ptr<T> AddComponent(const std::shared_ptr<Entity>& entity)
		{
			return std::static_pointer_cast<T>(AddComponent(entity, new T()));
		}
		ME_API void RemoveComponent(EntityID entityID, ComponentID componentID);

		ME_API void RemoveComponents(EntityID entityID);

		ME_API void UpdateComponents(float deltaTime);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(ComponentStorer);

		std::vector<std::vector<std::shared_ptr<Component>>> m_ComponentsByID; //Outer vector index: component id; Inner vector index: entity id;
	};
}