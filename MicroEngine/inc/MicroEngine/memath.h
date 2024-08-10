//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>
#include "me_interface.h"

constexpr float ME_RAD2DEG = 57.2957795f;
constexpr float ME_DEG2RAD = 0.0174532925f;
constexpr float ME_PI = 3.141592654f;

namespace me
{
	ME_API DirectX::XMVECTOR GetForwardVector(const DirectX::XMVECTOR& quaternion);
	ME_API DirectX::XMVECTOR GetRightVector(const DirectX::XMVECTOR& quaternion);
	ME_API DirectX::XMVECTOR GetUpVector(const DirectX::XMVECTOR& quaternion);

	template<typename T>
	T Clamp(const T &val, const T &min, const T &max)
	{
		return std::max(min, std::min(max, val));
	}
};