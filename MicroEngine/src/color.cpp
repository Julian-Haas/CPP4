//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "color.h"

namespace me
{

    Color Color::s_Black = Color{ 0, 0, 0, 1 };
    Color Color::s_White = Color{ 1, 1, 1, 1 };
    Color Color::s_Red = Color{ 1, 0, 0, 1 };
    Color Color::s_Green = Color{ 0, 1, 0, 1 };
    Color Color::s_Blue = Color{ 0, 0, 1, 1 };
    Color Color::s_Pink = Color{ 1, 0, 1, 1 };

    Color::Color(std::string hex)
    {
        if(hex.length() != 9)
            return;

        std::string r = hex.substr(1, 2);
        std::string g = hex.substr(3, 2);
        std::string b = hex.substr(5, 2);
        std::string a = hex.substr(7, 2);

        m_R = static_cast<float>(std::stoi(r, nullptr, 16)) / 255.f;
        m_G = static_cast<float>(std::stoi(g, nullptr, 16)) / 255.f;
        m_B = static_cast<float>(std::stoi(b, nullptr, 16)) / 255.f;
        m_A = static_cast<float>(std::stoi(a, nullptr, 16)) / 255.f;
    }
}
