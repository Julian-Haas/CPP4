//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "util.h"

struct D3D11_INPUT_ELEMENT_DESC;

namespace me
{
	struct VertexDescription
	{
		uint32_t m_VertexSize;
		uint32_t m_NumInputElements;
		D3D11_INPUT_ELEMENT_DESC* m_InputElementDesc;
	};

	/*struct VertexLitColored
	{
		float m_Position[3];
		float m_Normal[3];
		float m_Color[3];

		static VertexDescription s_Description;
	};*/

	struct VertexLitTextured
	{
		float m_Position[3];
		float m_Normal[3];
		float m_UV[2];
		static ME_API VertexDescription s_Description;
	};

	ME_STATIC_ASSERT(sizeof(VertexLitTextured) == 32);

	struct VertexTextured
	{
		float m_Position[3];
		float m_UV[2];

		static ME_API VertexDescription s_Description;
	};

	ME_STATIC_ASSERT(sizeof(VertexTextured) == 20);

};