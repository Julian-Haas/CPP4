//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "util.h"
#include "me_interface.h"

namespace me
{
	//If this were to be done properly we'd use raw input or direct input instead of windows messages...
	class Input
	{
	public:
		ME_API static Input* GetInstance();

		ME_API bool IsKeyDown(uint8_t virtualKey) const;
		ME_API bool IsKeyUp(uint8_t virtualKey) const;
		ME_API void GetMouseDelta(int& x, int& y) const;

		ME_API void OnFrameEnd();

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Input);
		Input();
		~Input() = default;

		friend class Window;

		void SetKeyDown(uint8_t virtualKey);
		void SetKeyUp(uint8_t virtualKey);
		void SetMousePosition(int x, int y);
		void ResetState();

		int m_LastMouseX;
		int m_LastMouseY;
		int m_MouseDeltaX;
		int m_MouseDeltaY;
	};
};