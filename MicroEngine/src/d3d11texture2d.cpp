//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include <memory>
#include "d3d11texture2d.h"

#include "color.h"
#include "d3d11device.h"
#include "d3d11util.h"
#include "directorywatcher.h"
#include "eventbus.h"
#include "xtktextureloader\WICTextureLoader.h"

namespace me
{
	D3D11Texture2D::D3D11Texture2D()
		: m_IsInit(false)
		, m_Width(0)
		, m_Height(0)
		, m_Format(DXGI_FORMAT_UNKNOWN)
		, m_BindFlags(0)
	{
		EventBus::GetInstance()->Register(FileChangedEvent::ID, reinterpret_cast<size_t>(this), [this](const std::shared_ptr<IEvent>& event)
		{
            const auto fileChangedEvent = std::static_pointer_cast<FileChangedEvent>(event);
			if (fileChangedEvent->m_FileName != m_File)
				return;

		    Release();
			if (fileChangedEvent->m_Removed)
			{
				Create(Color::s_Pink);
			}
			else
			{
                const std::string tmp = m_File;
			    CreateFromFile(m_File);
				m_File = tmp;
			}
		});
	}


	D3D11Texture2D::~D3D11Texture2D()
	{
		EventBus::GetInstance()->Unregister(FileChangedEvent::ID, reinterpret_cast<size_t>(this));
		Release();
	}

	void D3D11Texture2D::Release()
	{
		m_IsInit = false;
		m_SRV.Reset();
		m_RTV.Reset();
		m_DSV.Reset();
		m_Texture.Reset();
	}

	D3D11_TEXTURE2D_DESC CreateTextureDesc(int width, int height, DXGI_FORMAT format, uint32_t bindFlags)
	{
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = format;
		texDesc.Width = static_cast<UINT>(width);
		texDesc.Height = static_cast<UINT>(height);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.BindFlags = bindFlags;
		texDesc.CPUAccessFlags = 0;

		return texDesc;
	}

	bool D3D11Texture2D::Create(int width, int height, DXGI_FORMAT format, uint32_t bindFlags)
	{
		if (m_IsInit)
			return false;

		D3D11_TEXTURE2D_DESC texDesc = CreateTextureDesc(width, height, format, bindFlags);

		if (Failed(D3D11Device::GetDev()->CreateTexture2D(&texDesc, nullptr, m_Texture.GetPointerAddress())))
		{
			return false;
		}

		if (!CreateViews(bindFlags))
			return false;

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_BindFlags = bindFlags;
		m_IsInit = true;

		return true;
	}

	bool D3D11Texture2D::Create(const void* buffer, int width, int height, DXGI_FORMAT format, uint32_t bindFlags)
	{
		if (m_IsInit)
			return false;

		D3D11_TEXTURE2D_DESC texDesc = CreateTextureDesc(width, height, format, bindFlags);

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = buffer;
		srd.SysMemPitch = static_cast<UINT>(GetFormatBytes(format) * width);

		if (Failed(D3D11Device::GetDev()->CreateTexture2D(&texDesc, &srd, m_Texture.GetPointerAddress())))
		{
			return false;
		}

		if (!CreateViews(bindFlags))
			return false;

		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_BindFlags = bindFlags;
		m_IsInit = true;

		return true;
	}

	bool D3D11Texture2D::Create(const Color& color)
	{
		return Create(reinterpret_cast<const void*>(&color), 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_SHADER_RESOURCE);
	}

	bool D3D11Texture2D::CreateFromFile(const std::string& file)
	{
		{
			ComPtr<ID3D11Resource> resource;
			if (Failed(DirectX::CreateWICTextureFromFile(D3D11Device::GetDev(), D3D11Device::GetContext(), UTF8ToUTF16(file).c_str(), resource.GetPointerAddress(), m_SRV.GetPointerAddress())))
			{
				Create(Color::s_Pink);
				m_File = file;
				return false;
			}

			if (Failed(resource->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(m_Texture.GetPointerAddress()))))
			{
				Create(Color::s_Pink);
				m_File = file;
				return false;
			}
		}

		m_File = file;

		D3D11_TEXTURE2D_DESC texDesc = {};
		m_Texture->GetDesc(&texDesc);
		m_Width = texDesc.Width;
		m_Height = texDesc.Height;
		m_Format = texDesc.Format;
		m_BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_IsInit = true;
		return true;
	}

	bool D3D11Texture2D::InitFromExisting(ID3D11Texture2D* texture, uint32_t bindFlags)
	{
		ME_ASSERT(texture);

		*m_Texture.GetPointerAddress() = texture;

		D3D11_TEXTURE2D_DESC texDesc = {};
		texture->GetDesc(&texDesc);

		m_Width = texDesc.Width;
		m_Height = texDesc.Height;
		m_Format = texDesc.Format;
		m_BindFlags = bindFlags;

		if (!CreateViews(bindFlags))
			return false;

		return true;
	}

	bool D3D11Texture2D::CreateViews(uint32_t bindFlags)
	{
		if (IsFlagSet(bindFlags, D3D11_BIND_RENDER_TARGET))
		{
			if (Failed(D3D11Device::GetDev()->CreateRenderTargetView(m_Texture, nullptr, m_RTV.GetPointerAddress())))
			{
				ME_LOG_ERROR("Unable to create RTV");
				return false;
			}
		}

		if (IsFlagSet(bindFlags, D3D11_BIND_SHADER_RESOURCE))
		{
			if (Failed(D3D11Device::GetDev()->CreateShaderResourceView(m_Texture, nullptr, m_SRV.GetPointerAddress())))
			{
				ME_LOG_ERROR("Unable to create SRV");
				return false;
			}
		}

		if (IsFlagSet(bindFlags, D3D11_BIND_DEPTH_STENCIL))
		{
			if (Failed(D3D11Device::GetDev()->CreateDepthStencilView(m_Texture, nullptr, m_DSV.GetPointerAddress())))
			{
				ME_LOG_ERROR("Unable to create DSV");
				return false;
			}
		}

		return true;
	}
};