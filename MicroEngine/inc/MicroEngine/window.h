//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <WinDef.h>

#include "eventbus.h"
#include "util.h"
#include "me_interface.h"

namespace me
{
	class WindowSizeChangedEvent : public IEvent
	{
	public:
		WindowSizeChangedEvent(int width, int height, bool minimized)
			: m_Width(width)
		    , m_Height(height)
		    , m_Minimized(minimized)
		{}
		virtual ~WindowSizeChangedEvent() = default;
		static const EventID ID = EngineEventID::WindowSizeChanged;
		virtual EventID GetID() const override { return ID; }
		const int m_Width;;
		const int m_Height;
		const bool m_Minimized;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(WindowSizeChangedEvent);
	};

	class Window
	{
	public:
		ME_API Window(const std::string& windowName, HINSTANCE hInst);
		ME_API ~Window() = default;

		ME_API bool ProcessMessages();
		HWND GetHWnd() const { return m_HWnd; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Window);

		static LRESULT WINAPI StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WndProc(UINT, WPARAM, LPARAM);

		HWND m_HWnd;
		int m_Width;
		int m_Height;
	};

};