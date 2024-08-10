//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>
#include "util.h"
#include "component.h"
#include "me_interface.h"

namespace me
{
	//TODO: Support transform hierarchies. Child world transforms are then multiplied by parent world transforms.
	class TransformComponent : public Component
	{
	public:
		static const ComponentID ID = EngineComponentID::Transform;
		virtual ComponentID GetID() const override { return ID; }

		ME_API TransformComponent();
		ME_API virtual ~TransformComponent() = default;

		ME_API virtual void Update(float deltaTime) override;

		const DirectX::XMVECTOR& GetPosition() const { return m_Position; }
		const DirectX::XMVECTOR& GetRotation() const { return m_Rotation; }

		float GetScale() const { return m_Scale; }

		void SetPosition(float x, float y, float z) { SetPosition(DirectX::XMVectorSet(x, y, z, 0)); }
		ME_API void SetPosition(const DirectX::XMVECTOR& position);
		void SetRotation(float x, float y, float z) { SetRotation(DirectX::XMVectorSet(x, y, z, 0)); }
		ME_API void SetRotation(const DirectX::XMVECTOR& rotation);
		ME_API void SetScale(float factor);

		ME_API void TranslateLocal(float x, float y, float z);
		ME_API void TranslateLocal(const DirectX::XMVECTOR& translation); //translate relative to the entities local axis space/orientation
		ME_API void Translate(float x, float y, float z);
		ME_API void Translate(const DirectX::XMVECTOR& translation);
		ME_API void RotateLocal(float yaw, float pitch, float roll);
		ME_API void RotateLocal(const DirectX::XMVECTOR& rotationEuler); //rotate relative to the entities local axis space/orientation
		ME_API void Rotate(float yaw, float pitch, float roll);
		ME_API void Rotate(const DirectX::XMVECTOR& rotationEuler);
		ME_API void Scale(float factor);

		const DirectX::XMMATRIX& GetWorldMatrix() const { return m_WorldMatrix; }
		bool WasWorldDirty() const { return m_WasWorldDirty; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(TransformComponent);

		DirectX::XMVECTOR m_Rotation; //Quaternion to avoid gimbal lock problem with taditional yaw/pitch/Roll
		DirectX::XMVECTOR m_Position;
		DirectX::XMMATRIX m_WorldMatrix;
		float m_Scale;
		bool m_IsWorldDirty;
		bool m_WasWorldDirty;
	};

};