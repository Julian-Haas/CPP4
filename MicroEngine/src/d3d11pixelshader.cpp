//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "d3d11pixelshader.h"
#include "d3d11device.h"
#include "d3d11util.h"
#include "eventbus.h"
#include "directorywatcher.h"

namespace me
{

	D3D11PixelShader::D3D11PixelShader()
		: m_IsInit(false)
		, m_ShaderVersionMajor(0)
		, m_ShaderVersionMinor(0)
	    , m_NumTextures(0)
	{
		EventBus::GetInstance()->Register(FileChangedEvent::ID, reinterpret_cast<size_t>(this), [this](const std::shared_ptr<IEvent>& event)
		{
            const auto fileChangedEvent = std::static_pointer_cast<FileChangedEvent>(event);
			if (fileChangedEvent->m_FileName == m_File && EndsWith(m_File, ".hlsl"))
			{
				Release();
				CreateFromSource(m_File, m_EntryPoint, m_ShaderVersionMajor, m_ShaderVersionMinor, m_Properties.GetSize(), m_NumTextures);
			}
		});
	}


	D3D11PixelShader::~D3D11PixelShader()
	{
		EventBus::GetInstance()->Unregister(FileChangedEvent::ID, reinterpret_cast<size_t>(this));
	}

	void D3D11PixelShader::Release()
	{
		if (m_IsInit)
		{
			m_PixelShader = nullptr;
			m_Properties.Release();
			m_IsInit = false;
		}
	}

	bool D3D11PixelShader::CreateFromPrecompiled(ID3DBlob* psBlob, size_t settingsBufferSizeBytes)
	{
		if (m_IsInit)
			return false;

		if (Failed(D3D11Device::GetDev()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_PixelShader.GetPointerAddress())))
		{
			ME_LOG_ERROR("Unable to create pixel shader");
			return false;
		}

		if(settingsBufferSizeBytes > 0 && !m_Properties.CreateDynamic(settingsBufferSizeBytes, D3D11_BIND_CONSTANT_BUFFER))
			return false;

		m_IsInit = true;

		return true;
	}

	bool D3D11PixelShader::CreateFromPrecompiled(const std::string& file, size_t settingsBufferSizeBytes, int numTextures)
	{
		ME_ASSERTF(EndsWith(file, ".cso"), "Invalid file extension for precompiled pixel shader: %s", file.c_str());

		m_File = file;
		m_EntryPoint.clear();
		m_ShaderVersionMajor = 0;
		m_ShaderVersionMinor = 0;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> psBlob = ReadFileToBlob(file);
		if (!psBlob)
		{
			return false;
		}

		return CreateFromPrecompiled(psBlob, settingsBufferSizeBytes);
	}

	bool D3D11PixelShader::CreateFromSource(const std::string& file, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, size_t settingsBufferSizeBytes, int numTextures)
	{
		m_File = file;
		m_EntryPoint = entryPoint;
		m_ShaderVersionMajor = shaderVersionMajor;
		m_ShaderVersionMinor = shaderVersionMinor;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> psBlob = CompileShader(file, entryPoint, StringFormat("ps_%u_%u", shaderVersionMajor, shaderVersionMinor));
		if (!psBlob)
			return false;

		return CreateFromPrecompiled(psBlob, settingsBufferSizeBytes);
	}

	bool D3D11PixelShader::CreateFromSource(const void* src, std::size_t size, const std::string& name, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, size_t settingsBufferSizeBytes, int numTextures)
	{
	    m_File = name;
		m_EntryPoint = entryPoint;
		m_ShaderVersionMajor = shaderVersionMajor;
		m_ShaderVersionMinor = shaderVersionMinor;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> psBlob = CompileShader(src, size, name, entryPoint, StringFormat("ps_%u_%u", shaderVersionMajor, shaderVersionMinor));
		if (!psBlob)
			return false;

		return CreateFromPrecompiled(psBlob, settingsBufferSizeBytes);
	}
};