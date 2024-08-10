//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "cameracomponent.h"

#include "d3d11device.h"
#include "entity.h"
#include "memath.h"
#include "transformcomponent.h"
#include "window.h"

namespace me
{
	CameraComponent::CameraComponent()
		: m_ProjectionMatrix(DirectX::XMMatrixIdentity())
		, m_ViewMatrix(DirectX::XMMatrixIdentity())
		, m_ViewPort(D3D11_VIEWPORT{})
	    , m_NearPlane(0.3f)
	    , m_FarPlane(1000.0f)
	    , m_HorizontalFoV(70)
	    , m_Width(static_cast<float>(D3D11Device::GetInstance()->GetSwapchainWidth())) //TODO: ugly
	    , m_Height(static_cast<float>(D3D11Device::GetInstance()->GetSwapchainHeight())) //TODO: ugly
	    , m_IsProjectionDirty(true)
	{
		SetViewPortNormalized(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		EventBus::GetInstance()->Register(WindowSizeChangedEvent::ID, reinterpret_cast<size_t>(this), [this](const std::shared_ptr<IEvent>& event)
		{
            const auto ev = std::static_pointer_cast<WindowSizeChangedEvent>(event);
			m_Width = static_cast<float>(ev->m_Width);
			m_Height = static_cast<float>(ev->m_Height);
			m_IsProjectionDirty = true;
		});
	}

	CameraComponent::~CameraComponent()
	{
	    EventBus::GetInstance()->Unregister(WindowSizeChangedEvent::ID, reinterpret_cast<size_t>(this));
	}

    void CameraComponent::SetHorizontalFoV(float fov)
	{
		m_HorizontalFoV = fov;
		m_IsProjectionDirty = true;
	}

    void CameraComponent::SetClippingPlanes(float nearPlane, float farPlane)
	{
	    m_NearPlane = nearPlane;
		m_FarPlane = farPlane;
		m_IsProjectionDirty = true;
	}

	void CameraComponent::SetViewPortNormalized(float width, float height, float topLeftX, float topLeftY, float minDepth, float maxDepth)
	{
		ME_ASSERTF(width >= 0.01f && width <= 1.0f, "The normalized width must be between 0.01 and 1");
		ME_ASSERTF(height >= 0.01f && height <= 1.0f, "The normalized height must be between 0.01 and 1");
		ME_ASSERTF(topLeftX >= 0 && topLeftX <= 1.0f, "The normalized topLeftX offset must be between 0.0 and 1");
		ME_ASSERTF(topLeftY >= 0.0f && topLeftY <= 1.0f, "The normalized topLeftY offset must be between 0.0 and 1");
		ME_ASSERTF(topLeftX <= width, "The render area is (partially) outs of the bounds");
		ME_ASSERTF(topLeftY <= height, "The render area is (partially) outs of the bounds");

		m_ViewPort.Width = width;
		m_ViewPort.Height = height;
		m_ViewPort.MinDepth = minDepth;
		m_ViewPort.MaxDepth = maxDepth;
		m_ViewPort.TopLeftX = topLeftX;
		m_ViewPort.TopLeftY = topLeftY;
		m_IsProjectionDirty = true;
	}

	void CameraComponent::Update(float deltaTime)
	{
		using namespace DirectX;
		ME_UNUSED(deltaTime);

		if(m_IsProjectionDirty)
		{
            const float verticalFoV = 2 * atan(tan((m_HorizontalFoV * ME_DEG2RAD)/2) * (16.0f/9.0f)); //nobody wants vertical fov, use hor+!
		    m_ProjectionMatrix = XMMatrixPerspectiveFovLH(verticalFoV, CalculateAspectRatio(), m_NearPlane, m_FarPlane);
			m_IsProjectionDirty = false;
		}

        const auto transform = GetOwner()->GetComponent<TransformComponent>().lock();
		if (!transform)
			return;

		if (!transform->WasWorldDirty())
			return;

        const XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
        const XMVECTOR up = XMVectorSet(0, 1, 0, 0);

        const XMVECTOR position = transform->GetPosition();
        const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(transform->GetRotation());
        const XMVECTOR cameraForward = XMVector4Transform(forward, rotationMatrix);
        const XMVECTOR cameraUp = XMVector4Transform(up, rotationMatrix);

		m_ViewMatrix = XMMatrixLookAtLH(position, XMVectorAdd(position, cameraForward), cameraUp);
	}

	float CameraComponent::CalculateAspectRatio() const
	{
	    return (m_Width * m_ViewPort.Width) / (m_Height * m_ViewPort.Height);
	}
};