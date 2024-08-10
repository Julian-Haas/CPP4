//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include "comptr.h"
#include "d3d11buffer.h"
#include "util.h"
#include "me_interface.h"

namespace me
{
	class D3D11PixelShader
	{
	public:
		ME_API D3D11PixelShader();
		ME_API ~D3D11PixelShader();

		ME_API void Release();
		ME_API bool CreateFromPrecompiled(const std::string& file, size_t settingsBufferSizeBytes = SHADER_NO_SETTINGS_BUFFER, int numTextures = 0);
		ME_API bool CreateFromSource(const std::string& file, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, size_t settingsBufferSizeBytes = SHADER_NO_SETTINGS_BUFFER, int numTextures = 0);
		ME_API bool CreateFromSource(const void* src, std::size_t size, const std::string& name, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, size_t settingsBufferSizeBytes = SHADER_NO_SETTINGS_BUFFER, int numTextures = 0);

		D3D11Buffer* GetPropertiesBuffer() { return &m_Properties; }
		int GetNumTextures() const { return m_NumTextures; }
		bool IsInit() const { return m_IsInit; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(D3D11PixelShader);

		friend class D3D11Device;
		ID3D11PixelShader* GetShader() const { return m_PixelShader; }

		bool CreateFromPrecompiled(ID3DBlob* psBlob, size_t settingsBufferSizeBytes);

		bool m_IsInit;
		ComPtr<ID3D11PixelShader> m_PixelShader;
		std::string m_File;
		std::string m_EntryPoint;
		uint32_t m_ShaderVersionMajor;
		uint32_t m_ShaderVersionMinor;
		D3D11Buffer m_Properties;
		int m_NumTextures;
	};

};

