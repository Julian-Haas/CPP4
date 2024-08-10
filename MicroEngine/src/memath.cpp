//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "memath.h"

namespace me
{
	DirectX::XMVECTOR GetForwardVector(const DirectX::XMVECTOR& quaternion)
	{
		const float x = quaternion.m128_f32[0];
		const float y = quaternion.m128_f32[1];
		const float z = quaternion.m128_f32[2];
		const float w = quaternion.m128_f32[3];
		return DirectX::XMVector4Normalize(DirectX::XMVectorSet(2 * x * z - 2 * y * w, 2 * y * z + 2 * x * w, 1 - 2 * x * x - 2 * y * y, 0));
	};

	DirectX::XMVECTOR GetRightVector(const DirectX::XMVECTOR& quaternion)
	{
		const float x = quaternion.m128_f32[0];
		const float y = quaternion.m128_f32[1];
		const float z = quaternion.m128_f32[2];
		const float w = quaternion.m128_f32[3];
		return DirectX::XMVector4Normalize(DirectX::XMVectorSet(1 - 2 * y * y - 2 * z * z, 2 * x * y - 2 * z * w, 2 * x * z + 2 * y * w, 0));
	};

	DirectX::XMVECTOR GetUpVector(const DirectX::XMVECTOR& quaternion)
	{
		const float x = quaternion.m128_f32[0];
		const float y = quaternion.m128_f32[1];
		const float z = quaternion.m128_f32[2];
		const float w = quaternion.m128_f32[3];
		return DirectX::XMVector4Normalize(DirectX::XMVectorSet(2 * x * y + 2 * z * w, 1 - 2 * x * x - 2 * z * z, 2 * y * z - 2 * x * w, 0));
	};
};