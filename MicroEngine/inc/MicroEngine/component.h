//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "util.h"

namespace me
{
	//Order in the enum dictates update and messaging order!

	typedef uint32_t ComponentID;

	namespace EngineComponentID
	{
		enum Enum : ComponentID
		{
			FIRST,
			INVALID = FIRST,
			Transform,
			RendererComponent,
			FirstPersonController /*must be updated before camera*/,
			Camera,
			Light,
			COUNT
		};
	};



	class Entity;

	class Component
	{
	public:
		Component()
			: m_Owner(nullptr)
			, m_Enabled(true)
		{}
		virtual ~Component() = default;

		virtual ComponentID GetID() const = 0;
		virtual void Update(float deltaTime) { ME_UNUSED(deltaTime); };
		virtual void Init() {};
		virtual void OnMessage(int id) { ME_UNUSED(id); };
		const std::shared_ptr<Entity>& GetOwner() const { return m_Owner; }
		void SetEnabled(bool enable) { m_Enabled = enable; }
		bool IsEnabled() const { return m_Enabled; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Component);

		friend class ComponentStorer;
		void SetOwner(const std::shared_ptr<Entity>& entity) { m_Owner = entity; }

		std::shared_ptr<Entity> m_Owner;
		bool m_Enabled;
	};

};