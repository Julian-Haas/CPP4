//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "input.h"
#include "memath.h"

namespace me
{
	bool g_KeyStates[UINT8_MAX];

	/*static*/ Input* Input::GetInstance()
	{
		static Input s_Instance;
		return &s_Instance;
	}

	Input::Input()
	    : m_LastMouseX(0)
	    , m_LastMouseY(0)
	    , m_MouseDeltaX(0)
	    , m_MouseDeltaY(0)
	{
		ResetState();
	}

	void Input::OnFrameEnd()
	{
		m_MouseDeltaX = 0;
		m_MouseDeltaY = 0;
	}

	bool Input::IsKeyDown(uint8_t virtualKey) const
	{
		return g_KeyStates[virtualKey];
	}

	bool Input::IsKeyUp(uint8_t virtualKey) const
	{
		return !g_KeyStates[virtualKey];
	}

	void Input::GetMouseDelta(int& x, int& y) const
	{
		x = m_MouseDeltaX;
		y = m_MouseDeltaY;
	}

	void Input::SetKeyDown(uint8_t virtualKey)
	{
		g_KeyStates[virtualKey] = true;
	}

	void Input::SetKeyUp(uint8_t virtualKey)
	{
		g_KeyStates[virtualKey] = false;
	}

	void Input::SetMousePosition(int x, int y)
	{
        const int deltaX = Clamp(x - m_LastMouseX, -20, 20);
        const int deltaY = Clamp(y - m_LastMouseY, -20, 20);

		if (m_LastMouseX != INT32_MAX && m_LastMouseY != INT32_MAX && (deltaX != 0 || deltaY != 0))
		{
			m_MouseDeltaX = deltaX;
			m_MouseDeltaY = deltaY;
		}
		m_LastMouseX = x;
		m_LastMouseY = y;
	}

	void Input::ResetState()
	{
		m_MouseDeltaX = 0;
		m_MouseDeltaY = 0;
		m_LastMouseX = INT32_MAX;
		m_LastMouseY = INT32_MAX;

		memset(g_KeyStates, 0, sizeof(g_KeyStates));
	}
};