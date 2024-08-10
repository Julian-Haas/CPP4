//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "d3d11buffer.h"
#include "d3d11device.h"

namespace me
{

	D3D11Buffer::D3D11Buffer()
		: m_IsInit(false)
		, m_IsDynamic(false)
		, m_Size(0)
	{
	}

	bool D3D11Buffer::CreateStatic(const void * data, size_t sizeBytes, uint32_t bindFlags)
	{
		if (m_IsInit)
			return false;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeBytes);
		bufferDesc.BindFlags = bindFlags;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = data;

		if (Failed(D3D11Device::GetDev()->CreateBuffer(&bufferDesc, &subresourceData, m_Buffer.GetPointerAddress())))
			return false;

		m_IsInit = true;
		m_Size = sizeBytes;

		return true;
	}

	bool D3D11Buffer::CreateDynamic(size_t sizeBytes, uint32_t bindFlags)
	{
		if (m_IsInit)
			return false;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = static_cast<UINT>(sizeBytes);
		bufferDesc.BindFlags = bindFlags;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (Failed(D3D11Device::GetDev()->CreateBuffer(&bufferDesc, nullptr, m_Buffer.GetPointerAddress())))
			return false;

		m_IsInit = true;
		m_IsDynamic = true;
		m_Size = sizeBytes;

		return true;
	}

	void D3D11Buffer::Release()
	{
		m_Buffer = nullptr;

		m_IsInit = false;
		m_IsDynamic = false;
		m_Size = 0;
	}

	bool D3D11Buffer::Write(const void* data, size_t size)
	{
		if (!m_IsDynamic)
		{
			ME_ASSERTF(false, "Cannot write to static buffer");
			return false;
		}

		if (size > m_Size)
		{
			ME_ASSERTF(false, "Trying to write more than buffers capacity");
			return false;
		}

		ID3D11DeviceContext* context = D3D11Device::GetContext();
		D3D11_MAPPED_SUBRESOURCE mappedConstantBufferSubResource = {};
		if (Failed(context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedConstantBufferSubResource)))
			return false;

		memcpy(mappedConstantBufferSubResource.pData, data, size);
		mappedConstantBufferSubResource.DepthPitch = 0;
		mappedConstantBufferSubResource.RowPitch = 0;
		context->Unmap(m_Buffer, 0);

		return true;
	}

};
