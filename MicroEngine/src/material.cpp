//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "material.h"

namespace me
{
	Material::Material()
    {}

    void Material::AddShaderProperty(const Color& color)
	{
		const float* col = color;
	    m_ShaderProperties.insert(m_ShaderProperties.end(), col, col + 4);
	}

    void Material::AddShaderProperty(float val1, float val2, float val3, float val4)
	{
	    m_ShaderProperties.push_back(val1);
	    m_ShaderProperties.push_back(val2);
	    m_ShaderProperties.push_back(val3);
	    m_ShaderProperties.push_back(val4);
	}

    void Material::SetTextureVS(std::size_t slot, const TextureInfo& textureInfo)
    {
        m_TexturesVS.resize(slot+1);
        m_TexturesVS[slot] = textureInfo;
    }

	void Material::SetTexturePS(std::size_t slot, const TextureInfo& textureInfo)
    {
        m_TexturesPS.resize(slot+1);
        m_TexturesPS[slot] = textureInfo;
    }
};