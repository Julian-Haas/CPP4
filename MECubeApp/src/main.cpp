//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "cubeapp.h"
#include "MicroEngine\windowsinc.h"
#include "MicroEngine\util.h"
#include "MicroEngine\d3d11device.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "MicroEngine\libraries.h"

#pragma comment(lib, "MicroEngine_" CONFIG_NAME ".lib")

int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nCmdShow)
{
	ME_UNUSED(nCmdShow);
	ME_UNUSED(lpCmdLine);
	ME_UNUSED(hPrevInstance);

	std::wstring currentDir = __wargv[0];
    const size_t index = currentDir.find_last_of(L'\\');
	currentDir.erase(index);
	SetCurrentDirectoryW(currentDir.c_str());

	capp::CubeApp app;
	return app.Run(hInst);
}