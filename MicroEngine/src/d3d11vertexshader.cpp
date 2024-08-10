//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include <d3dcompiler.h>
#include "d3d11vertexshader.h"
#include "d3d11device.h"
#include "vertex.h"
#include "eventbus.h"
#include "d3d11util.h"
#include "directorywatcher.h"

namespace me
{

	D3D11VertexShader::D3D11VertexShader()
		: m_IsInit(false)
		, m_ShaderVersionMajor(0)
		, m_ShaderVersionMinor(0)
		, m_VertexDescription(VertexDescription{})
	    , m_NumTextures(0)
	{
		EventBus::GetInstance()->Register(FileChangedEvent::ID, reinterpret_cast<size_t>(this), [this](const std::shared_ptr<IEvent>& event)
		{
            const auto fileChangedEvent = std::static_pointer_cast<FileChangedEvent>(event);
			if (fileChangedEvent->m_FileName == m_File && EndsWith(m_File, ".hlsl"))
			{
				Release();
				CreateFromSource(m_File, m_EntryPoint, m_ShaderVersionMajor, m_ShaderVersionMinor, m_VertexDescription, m_Properties.GetSize(), m_NumTextures);
			}
		});
	}


	D3D11VertexShader::~D3D11VertexShader()
	{
		EventBus::GetInstance()->Unregister(FileChangedEvent::ID, reinterpret_cast<size_t>(this));
	}

	void D3D11VertexShader::Release()
	{
		if (m_IsInit)
		{
			m_VertexShader = nullptr;
			m_InputLayout = nullptr;
			m_Properties.Release();
			m_IsInit = false;
		}
	}

	bool D3D11VertexShader::CreateFromPrecompiled(ID3DBlob* vsBlob, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes)
	{
		if (m_IsInit)
			return false;

		if (Failed(D3D11Device::GetDev()->CreateInputLayout(vertexDescription.m_InputElementDesc, static_cast<UINT>(vertexDescription.m_NumInputElements), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_InputLayout.GetPointerAddress())))
		{
			ME_LOG_ERROR("Unable to create input layout");
			return false;
		}

		if (Failed(D3D11Device::GetDev()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_VertexShader.GetPointerAddress())))
		{
			ME_LOG_ERROR("Unable to create vertex shader");
			return false;
		}

		if(settingsBufferSizeBytes > 0 && !m_Properties.CreateDynamic(settingsBufferSizeBytes, D3D11_BIND_CONSTANT_BUFFER))
			return false;

		m_IsInit = true;

		return true;
	}

	bool D3D11VertexShader::CreateFromPrecompiled(const std::string& file, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes, int numTextures)
	{
		ME_ASSERTF(EndsWith(file, ".cso"), "Invalid file extension for precompiled vertex shader: %s", file.c_str());

		m_File = file;
		m_EntryPoint.clear();
		m_ShaderVersionMajor = 0;
		m_ShaderVersionMinor = 0;
		m_VertexDescription = vertexDescription;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> vsBlob = ReadFileToBlob(file);
		if (!vsBlob)
		{
			return false;
		}

		return CreateFromPrecompiled(vsBlob, vertexDescription, settingsBufferSizeBytes);
	}

	bool D3D11VertexShader::CreateFromSource(const std::string& file, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes, int numTextures)
	{
		m_File = file;
		m_EntryPoint = entryPoint;
		m_ShaderVersionMajor = shaderVersionMajor;
		m_ShaderVersionMinor = shaderVersionMinor;
		m_VertexDescription = vertexDescription;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> vsBlob = CompileShader(file, entryPoint, StringFormat("vs_%u_%u", shaderVersionMajor, shaderVersionMinor));
		if (!vsBlob)
			return false;

		return CreateFromPrecompiled(vsBlob, vertexDescription, settingsBufferSizeBytes);
	}

	bool D3D11VertexShader::CreateFromSource(const void* src, std::size_t size, const std::string& name, const std::string& entryPoint, uint32_t shaderVersionMajor, uint32_t shaderVersionMinor, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes, int numTextures)
	{
	    m_File = name;
		m_EntryPoint = entryPoint;
		m_ShaderVersionMajor = shaderVersionMajor;
		m_ShaderVersionMinor = shaderVersionMinor;
		m_VertexDescription = vertexDescription;
		m_NumTextures = numTextures;

		ComPtr<ID3DBlob> vsBlob = CompileShader(src, size, name, entryPoint, StringFormat("vs_%u_%u", shaderVersionMajor, shaderVersionMinor));
		if (!vsBlob)
			return false;

		return CreateFromPrecompiled(vsBlob, vertexDescription, settingsBufferSizeBytes);
	}
};