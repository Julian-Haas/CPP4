//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "entity.h"
#include "memath.h"
#include "componentstorer.h"

namespace me
{
	Entity::Entity(ComponentStorer& componentStorer, EntityID id)
		: m_ID(id)
		, m_ComponentStorer(componentStorer)
	{
	}

	std::weak_ptr<Component> Entity::GetComponent(ComponentID componentID) const
	{
		return m_ComponentStorer.GetComponent(m_ID, componentID);
	}

	void Entity::SendMessage(int id) const
    {
		for (auto& component : m_ComponentStorer.GetComponents(m_ID))
		{
			component->OnMessage(id);
		}
	}
};