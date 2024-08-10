//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#if defined(TARGET_DEBUG)
#define CONFIG_NAME "Debug"
#elif defined(TARGET_RELEASE)
#define CONFIG_NAME "Release"
#elif defined(TARGET_RETAIL)
#define CONFIG_NAME "Retail"
#endif

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")