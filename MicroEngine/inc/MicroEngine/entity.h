//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "component.h"
#include "componentstorer.h"
#include "util.h"
#include "me_interface.h"

namespace me
{
	class Entity
	{
	public:
		ME_API Entity(ComponentStorer& componentStorer, EntityID id);
		ME_API ~Entity() = default;

		EntityID GetID() const { return m_ID; }

		ME_API std::weak_ptr<Component> GetComponent(ComponentID componentID) const;

		template <class T>
		std::weak_ptr<T> GetComponent() const { return std::static_pointer_cast<T>(m_ComponentStorer.GetComponent(m_ID, T::ID).lock()); }

		ME_API void SendMessage(int id) const;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Entity);

		EntityID m_ID;
		ComponentStorer& m_ComponentStorer;
	};

};