//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "window.h"
#include "windowsinc.h"
#include "input.h"
#include "eventbus.h"

namespace me
{
	Window::Window(const std::string& windowName, HINSTANCE hInst)
		: m_HWnd(nullptr)
		, m_Width(0)
		, m_Height(0)
	{
        const std::wstring windowNameUTF16 = UTF8ToUTF16(windowName);
		std::wstring className = windowNameUTF16;
		className.append(L"Class");

		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = StaticWndProc;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszClassName = className.c_str();
		wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

		RegisterClassEx(&wc);

        const DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		//Pass this instance in last param to LPCREATESTRUCT, used by StaticWndProc
		CreateWindowEx(0, wc.lpszClassName, windowNameUTF16.c_str(), windowStyle, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, this);

		DEVMODE	devmode = {};
		EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devmode);

		m_Width = static_cast<int>(devmode.dmPelsWidth * 0.7f);
		m_Height = static_cast<int>(devmode.dmPelsHeight * 0.7f);

        const int left = (static_cast<int>(devmode.dmPelsWidth) - m_Width) / 2;
        const int top = (static_cast<int>(devmode.dmPelsHeight) - m_Height) / 2;
		RECT rc = {};
		SetRect(&rc, left, top, left + m_Width, top + m_Height);
		AdjustWindowRectEx(&rc, windowStyle, FALSE, 0);

		MoveWindow(m_HWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

		ShowWindow(m_HWnd, SW_SHOW);
	}

	LRESULT WINAPI Window::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//Some magic to be able to forward the windows proc from a global function to a member function
		Window* parent;
		if (msg == WM_CREATE) {
			parent = static_cast<Window*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(parent));
		}
		else {
			parent = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (!parent)
				return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		parent->m_HWnd = hWnd;
		return parent->WndProc(msg, wParam, lParam);
	}

	LRESULT WINAPI Window::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		//Some very simple input handling with many issues, a proper solution would be to use direct input or raw input
		case WM_KEYDOWN:
			Input::GetInstance()->SetKeyDown(static_cast<uint8_t>(wParam));
			return 0;
		case WM_KEYUP:
			Input::GetInstance()->SetKeyUp(static_cast<uint8_t>(wParam));
			return 0;
		case WM_MOUSEMOVE:
		{
            const int x = static_cast<short>(LOWORD(lParam));
            const int y = static_cast<short>(HIWORD(lParam));
			Input::GetInstance()->SetMousePosition(x, y);			
			return 0;
		}
		case WM_LBUTTONDOWN:
			Input::GetInstance()->SetKeyDown(VK_LBUTTON);
			return 0;
		case WM_LBUTTONUP:
			Input::GetInstance()->SetKeyUp(VK_LBUTTON);
			return 0;
		case WM_RBUTTONDOWN:
			Input::GetInstance()->SetKeyDown(VK_RBUTTON);
			return 0;
		case WM_RBUTTONUP:
			Input::GetInstance()->SetKeyUp(VK_RBUTTON);
			return 0;
		case WM_MBUTTONDOWN:
			Input::GetInstance()->SetKeyDown(VK_MBUTTON);
			return 0;
		case WM_MBUTTONUP:
			Input::GetInstance()->SetKeyUp(VK_MBUTTON);
			return 0;
		case WM_KILLFOCUS:
			Input::GetInstance()->ResetState();
			break;

		//Handle window size change
		case WM_SIZE:
		{
			RECT clientRect = {};
			GetClientRect(m_HWnd, &clientRect);
		    int width = std::max(clientRect.right - clientRect.left, 64l);
		    int height = std::max(clientRect.bottom - clientRect.top, 64l);
			EventBus::GetInstance()->RaiseEvent(std::make_shared<WindowSizeChangedEvent>(width, height, wParam == SIZE_MINIMIZED));
			return 0;
		}

		//Handle window closing
		case WM_CLOSE:
			DestroyWindow(m_HWnd);	//  Sends us a WM_DESTROY
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			break;
		}
		return DefWindowProc(m_HWnd, msg, wParam, lParam);
	}

	bool Window::ProcessMessages()
	{
		MSG _msg = {};

		bool open = true;
		while (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_msg);
			DispatchMessage(&_msg);
			if (_msg.message == WM_QUIT) {
				open = false;
			}
		}

		return open;
	}
};