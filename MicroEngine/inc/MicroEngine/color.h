//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "me_interface.h"

namespace me
{

	struct ME_API Color
	{
		Color()
			: m_R(1.0f)
			, m_G(1.0f)
			, m_B(1.0f)
			, m_A(1.0f)
		{}

		Color(float r, float g, float b, float a = 1.0f)
			: m_R(r)
			, m_G(g)
			, m_B(b)
			, m_A(a)
		{}

	    Color(std::string hex);

		float m_R;
		float m_G;
		float m_B;
		float m_A;

		operator const float*() const
		{
			return &m_R;
		}

		static Color s_Black;
		static Color s_White;
		static Color s_Red;
		static Color s_Green;
		static Color s_Blue;
		static Color s_Pink;
	};

};