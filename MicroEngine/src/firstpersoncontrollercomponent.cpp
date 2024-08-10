//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "firstpersoncontrollercomponent.h"
#include "input.h"
#include "entity.h"
#include "memath.h"
#include "transformcomponent.h"

namespace me
{

	FirstPersonControllerComponent::FirstPersonControllerComponent()
		: m_TotalPitch(0.0f)
		, m_MovementSpeed(8.0f)
		, m_RotationSpeed(10.0f)
	{

	}

	void FirstPersonControllerComponent::Update(float deltaTime)
	{
		Input* input = Input::GetInstance();
		auto transform = GetOwner()->GetComponent<TransformComponent>().lock();
		if (!transform)
			return;

		//Handle camera movement
		{
			if (input->IsKeyDown(VK_UP) || input->IsKeyDown('W') || (input->IsKeyDown(VK_LBUTTON) && input->IsKeyDown(VK_RBUTTON)))
				transform->TranslateLocal(0, 0, m_MovementSpeed * deltaTime);
			if (input->IsKeyDown(VK_DOWN) || input->IsKeyDown('S'))
				transform->TranslateLocal(0, 0, -m_MovementSpeed * deltaTime);
			if (input->IsKeyDown(VK_RIGHT) || input->IsKeyDown('D'))
				transform->TranslateLocal(m_MovementSpeed * deltaTime, 0, 0);
			if (input->IsKeyDown(VK_LEFT) || input->IsKeyDown('A'))
				transform->TranslateLocal(-m_MovementSpeed * deltaTime, 0, 0);
			if (input->IsKeyDown(VK_NEXT /*page down*/) || input->IsKeyDown('Q'))
				transform->Translate(0, -m_MovementSpeed * deltaTime, 0);
			if (input->IsKeyDown(VK_PRIOR /* page up */) || input->IsKeyDown('E'))
				transform->Translate(0, m_MovementSpeed * deltaTime, 0);
		}

		//Handle camera orientation
		if(input->IsKeyDown(VK_LBUTTON))
		{
			int mouseDeltaX;
			int mouseDeltaY;
			input->GetMouseDelta(mouseDeltaX, mouseDeltaY);

			float pitch = (static_cast<float>(mouseDeltaY) * m_RotationSpeed * deltaTime);
            const float yaw = (static_cast<float>(mouseDeltaX) * m_RotationSpeed * deltaTime);
			m_TotalPitch += pitch;

            const float deg90 = 90.0f;

			if (m_TotalPitch > deg90) {
				pitch = deg90 - (m_TotalPitch - pitch);
				m_TotalPitch = deg90;
			}
			else if (m_TotalPitch < -deg90) {
				pitch = -deg90 - (m_TotalPitch - pitch);
				m_TotalPitch = -deg90;
			}
			if (yaw != 0.0f)
				transform->Rotate(0, yaw, 0);
			if (pitch != 0.0f)
				transform->RotateLocal(pitch, 0, 0);
		}
	}
};