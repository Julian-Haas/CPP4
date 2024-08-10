//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "lightcomponent.h"
#include "transformcomponent.h"
#include "entity.h"

namespace me
{

    LightComponent::LightComponent()
        : m_LightData{}
    {
        m_LightData.m_Position = DirectX::XMVectorSet(0, 0, 0, 1);
        const float range = 10;
	    m_LightData.m_Direction = DirectX::XMVectorSet(0, 0, 1, range);
	    m_LightData.m_Color = Color::s_White;
		m_LightData.m_Type = LightType::Directional;
		m_LightData.m_Intensity = 0.5f;
	    m_LightData.m_SpotAngleInner = cos((ME_DEG2RAD * 40.0f)/2.0f);
	    m_LightData.m_SpotAngleOuter = cos((ME_DEG2RAD * 50.0f)/2.0f);
    }

    void LightComponent::SetRange(float range)
	{
		ME_ASSERT(m_LightData.m_Type != LightType::Directional);
        m_LightData.m_Direction.m128_f32[3] = range;
	}

	void LightComponent::SetSpotlightAngles(float innerAngleDegrees, float outerAngleDegrees)
	{
		ME_ASSERT(m_LightData.m_Type == LightType::Spot);
		m_LightData.m_SpotAngleInner = cos((ME_DEG2RAD * innerAngleDegrees)/2.0f);
		m_LightData.m_SpotAngleOuter = cos((ME_DEG2RAD * outerAngleDegrees)/2.0f);
	}

	void LightComponent::Update(float deltaTime)
	{
		using namespace DirectX;
		ME_UNUSED(deltaTime);
        const auto transform = GetOwner()->GetComponent<TransformComponent>().lock();
		if (!transform)
			return;

		if (!transform->WasWorldDirty())
			return;

        m_LightData.m_Position = transform->GetPosition();
		const XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
        const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(transform->GetRotation());

        const float range = m_LightData.m_Direction.m128_f32[3];
        m_LightData.m_Direction = XMVector4Transform(forward, rotationMatrix);
		m_LightData.m_Direction.m128_f32[3] = range;
	}
}
