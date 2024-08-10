//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "component.h"
#include "me_interface.h"

namespace me
{

	class CameraComponent : public Component
	{
	public:
		ME_API CameraComponent();
		ME_API ~CameraComponent();

		static const ComponentID ID = EngineComponentID::Camera;
		virtual ComponentID GetID() const override { return ID; }

		ME_API virtual void Update(float deltaTime) override;

		const DirectX::XMMATRIX& GetProjection() const { return m_ProjectionMatrix; }
		ME_API void SetHorizontalFoV(float fov);
		ME_API void SetClippingPlanes(float nearPlane, float farPlane);
		float GetNearPlane() const { return m_NearPlane; }
		float GetFarPlane() const { return m_FarPlane; }

		const DirectX::XMMATRIX& GetView() const { return m_ViewMatrix; }

		const D3D11_VIEWPORT& GetViewPort() const { return m_ViewPort; }
		ME_API void SetViewPortNormalized(float width, float height, float topLeftX, float topLeftY, float minDepth = 0.0f, float maxDepth = 1.0f);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(CameraComponent);
		float CalculateAspectRatio() const;

		DirectX::XMMATRIX m_ProjectionMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		D3D11_VIEWPORT m_ViewPort;
		float m_NearPlane;
		float m_FarPlane;
		float m_HorizontalFoV;
		float m_Width;
		float m_Height;
		bool m_IsProjectionDirty;
	};

};