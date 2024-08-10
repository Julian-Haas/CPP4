//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#if defined(DLL_NO_EXPORT)
#define ME_API
#elif defined(DLL_EXPORT_MicroEngine)
#define ME_API __declspec(dllexport)
#else
#define ME_API __declspec(dllimport)
#endif