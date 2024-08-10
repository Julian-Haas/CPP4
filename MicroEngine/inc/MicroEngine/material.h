//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <utility>


#include "color.h"
#include "me_interface.h"
#include "util.h"

namespace me
{
	class TextureInfo
	{
	public:
		TextureInfo()
		    : m_Clamp(false)
		{}

		TextureInfo(std::string name, bool clamp = false)
		    : m_Name(std::move(name))
	        , m_Clamp(clamp)
		{}

		const std::string& GetName() const { return m_Name; }
		bool IsClamp() const { return m_Clamp; }
	private:
		std::string m_Name;
		bool m_Clamp;
	};

	class Material
	{
	public:
		ME_API Material();
		ME_API ~Material() = default;
		ME_API Material(const Material& other) = default;
		ME_API Material& operator=(const Material& other) = default;

		const std::string& GetVertexShader() const { return m_VertexShader; }
		const std::string& GetPixelShader() const { return m_PixelShader; }
		void SetVertexShader(const std::string& shader) { m_VertexShader = shader; }
		void SetPixelShader(const std::string& shader) { m_PixelShader = shader; }

		ME_API void AddShaderProperty(const Color& color);
		ME_API void AddShaderProperty(float val1, float val2 = 0.0f, float val3 = 0.0f, float val4 = 0.0f);
		ME_API const std::vector<float>& GetShaderProperties() { return m_ShaderProperties; }

		ME_API void SetTextureVS(std::size_t slot, const TextureInfo& textureInfo);
		ME_API void SetTexturePS(std::size_t slot, const TextureInfo& textureInfo);
		const std::vector<TextureInfo>& GetTexturesVS() const { return m_TexturesVS; }
		const std::vector<TextureInfo>& GetTexturesPS() const { return m_TexturesPS; }

	private:
		std::string m_VertexShader;
		std::string m_PixelShader;

		std::vector<float> m_ShaderProperties;
		std::vector<TextureInfo> m_TexturesVS;
		std::vector<TextureInfo> m_TexturesPS;
	};

};