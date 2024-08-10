//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include "comptr.h"
#include "me_interface.h"

namespace me
{
	class D3D11SamplerState
	{
	public:
		ME_API D3D11SamplerState();
		ME_API ~D3D11SamplerState() = default;

		ME_API bool Init(D3D11_FILTER Filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_COMPARISON_FUNC compare, uint32_t maxAnisotropy, float minLOD, float maxLOD, float mipLODBias);

		ME_API void Release();

		bool IsInit() const { return m_IsInit; }

		static const D3D11_FILTER POINT_FILTER = D3D11_FILTER_MIN_MAG_MIP_POINT;
		static const D3D11_FILTER BILINEAR_FILTER = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		static const D3D11_FILTER TRILINEAR_FILTER = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		static const D3D11_FILTER ANISOTROPIC_FILTER = D3D11_FILTER_ANISOTROPIC;
	private:
		ME_MOVE_COPY_NOT_ALLOWED(D3D11SamplerState);

		friend class D3D11Device;
		ID3D11SamplerState* GetSamplerState() const { return m_SamplerState; }

		bool m_IsInit;
		ComPtr<ID3D11SamplerState> m_SamplerState;
	};

}