//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "component.h"
#include "me_interface.h"

namespace me
{
	class FirstPersonControllerComponent : public Component
	{
	public:

		static const ComponentID ID = EngineComponentID::FirstPersonController;
		virtual ComponentID GetID() const override { return ID; }

		ME_API FirstPersonControllerComponent();
		ME_API virtual ~FirstPersonControllerComponent() = default;

		ME_API virtual void Update(float deltaTime) override;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(FirstPersonControllerComponent);
		float m_TotalPitch;
		float m_MovementSpeed;
		float m_RotationSpeed;
	};

};