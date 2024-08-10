//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "transformcomponent.h"
#include "memath.h"

namespace me
{

	TransformComponent::TransformComponent()
		: m_Rotation(DirectX::XMQuaternionIdentity())
		, m_Position(DirectX::XMVectorZero())
		, m_WorldMatrix(DirectX::XMMatrixIdentity())
		, m_Scale(1.0f)
		, m_IsWorldDirty(false)
		, m_WasWorldDirty(false)
	{}

	void TransformComponent::Update(float deltaTime)
	{
		using namespace DirectX;
		ME_UNUSED(deltaTime);
		m_WasWorldDirty = false;

		if (m_IsWorldDirty)
		{
			//Build SRT matrix
			m_WorldMatrix = XMMatrixScaling(m_Scale, m_Scale, m_Scale) * XMMatrixRotationQuaternion(m_Rotation) *  XMMatrixTranslationFromVector(m_Position);
			m_IsWorldDirty = false;
			m_WasWorldDirty = true;
		}
	}

	void TransformComponent::SetPosition(const DirectX::XMVECTOR& position)
	{
		m_IsWorldDirty = true;
		m_Position = position;
	}

	void TransformComponent::SetRotation(const DirectX::XMVECTOR& rotation)
	{
		using namespace DirectX;
		m_IsWorldDirty = true;
		m_Rotation = XMQuaternionRotationRollPitchYawFromVector(rotation * ME_DEG2RAD);
	}

	void TransformComponent::SetScale(float factor)
	{
		m_IsWorldDirty = true;
		m_Scale = factor;
	}

	void TransformComponent::TranslateLocal(float x, float y, float z)
	{
		using namespace DirectX;
	    TranslateLocal(XMVectorSet(x, y, z, 0));
	}

	void TransformComponent::TranslateLocal(const DirectX::XMVECTOR& translation)
	{
		using namespace DirectX;
        const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(m_Rotation);
		Translate(XMVector4Transform(translation, rotationMatrix));
	}

	void TransformComponent::Translate(float x, float y, float z)
	{
		using namespace DirectX;
	    Translate(XMVectorSet(x, y, z, 0));
	}

	void TransformComponent::Translate(const DirectX::XMVECTOR& translation)
	{
		using namespace DirectX;
		m_IsWorldDirty = true;
		m_Position = XMVectorAdd(m_Position, translation);
	}

	void TransformComponent::RotateLocal(float yaw, float pitch, float roll)
	{
		using namespace DirectX;
	    RotateLocal(XMVectorSet(yaw, pitch, roll, 0));
	}

	void TransformComponent::RotateLocal(const DirectX::XMVECTOR& rotation)
	{
		using namespace DirectX;
		m_IsWorldDirty = true;
		m_Rotation = XMQuaternionMultiply(XMQuaternionRotationRollPitchYawFromVector(rotation * ME_DEG2RAD), m_Rotation);
	}

	void TransformComponent::Rotate(float yaw, float pitch, float roll)
	{
		using namespace DirectX;
	    Rotate(XMVectorSet(yaw, pitch, roll, 0));
	}

	void TransformComponent::Rotate(const DirectX::XMVECTOR& rotation)
	{
		using namespace DirectX;
		m_IsWorldDirty = true;
		m_Rotation = XMQuaternionMultiply(m_Rotation, XMQuaternionRotationRollPitchYawFromVector(rotation * ME_DEG2RAD));
	}

	void TransformComponent::Scale(float factor)
	{
		m_IsWorldDirty = true;
		m_Scale *= factor;
	}

};