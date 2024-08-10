//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <DirectXCollision.h>


#include "color.h"
#include "component.h"
#include "memath.h"
#include "me_interface.h"

namespace me
{
	namespace LightType
	{
	    enum Enum : uint32_t
	    {
	        Invalid,
			Directional,
			Point,
			Spot
	    };
	}

	//Must be 32 byte aligned
	struct LightData
	{
	    DirectX::XMVECTOR m_Position; //only point and spot
		DirectX::XMVECTOR m_Direction; //only directional and spot, w contains range
		Color m_Color;
		LightType::Enum m_Type;
		float m_Intensity;
		float m_SpotAngleInner; //only spot
		float m_SpotAngleOuter; //only spot
	};

	class LightComponent : public Component
	{
	public:
		ME_API LightComponent();
		ME_API ~LightComponent() = default;

		static const ComponentID ID = EngineComponentID::Light;
		virtual ComponentID GetID() const override { return ID; }
		virtual void Update(float deltaTime) override;

		void SetType(LightType::Enum type) { m_LightData.m_Type = type; }
		void SetColor(const Color& color) { m_LightData.m_Color = color; }
		ME_API void SetRange(float range);
		ME_API void SetSpotlightAngles(float innerAngleDegrees, float outerAngleDegrees);
		void SetIntensity(float intensity) { m_LightData.m_Intensity = intensity; }

		const LightData& GetData() const { return m_LightData; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(LightComponent);
		LightData m_LightData;
	};

};