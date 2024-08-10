//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include "comptr.h"
#include "util.h"
#include "me_interface.h"

namespace me
{
    struct Color;

    class D3D11Texture2D
	{
	public:
		ME_API D3D11Texture2D();
		ME_API ~D3D11Texture2D();

		ME_API void Release();
		ME_API bool Create(int width, int height, DXGI_FORMAT format, uint32_t bindFlags);
		ME_API bool Create(const void* buffer, int width, int height, DXGI_FORMAT format, uint32_t bindFlags);
		ME_API bool Create(const Color& color);
		ME_API bool CreateFromFile(const std::string& file);
		ME_API bool InitFromExisting(ID3D11Texture2D* texture, uint32_t bindFlags);

		bool IsInit() const { return m_IsInit; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		uint32_t GetBindFlags() const { return m_BindFlags; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(D3D11Texture2D);

		friend class D3D11Device;
		ID3D11RenderTargetView* GetRTV() { return m_RTV; }
		ID3D11ShaderResourceView* GetSRV() { return m_SRV; }
		ID3D11DepthStencilView* GetDSV() { return m_DSV; }

		bool CreateViews(uint32_t bindFlags);

		bool m_IsInit;
		std::string m_File;
		int m_Width;
		int m_Height;
		DXGI_FORMAT m_Format;
		uint32_t m_BindFlags;
		ComPtr<ID3D11RenderTargetView> m_RTV;
		ComPtr<ID3D11ShaderResourceView> m_SRV;
		ComPtr<ID3D11DepthStencilView> m_DSV;
		ComPtr<ID3D11Texture2D> m_Texture;
	};

};