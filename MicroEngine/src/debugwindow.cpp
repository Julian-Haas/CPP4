#include "stdafx.h"
#include "debugwindow.h"
#include <Windows.h>   // For AllocConsole
#include <cstdio>      // For freopen_s
namespace me {
	void Debugwindow::OpenDebugConsole()
	{
		AllocConsole();
		FILE* file;
		freopen_s(&file, "CONOUT$", "w", stdout);
		freopen_s(&file, "CONOUT$", "w", stderr);
		freopen_s(&file, "CONIN$", "r", stdin);
	}

	void Debugwindow::CloseDebugConsole()
	{
		fclose(stdout);
		fclose(stderr);
		fclose(stdin);
		FreeConsole();
	}
}
