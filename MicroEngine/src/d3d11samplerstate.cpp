//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "d3d11samplerstate.h"
#include "d3d11device.h"

namespace me
{
	D3D11SamplerState::D3D11SamplerState()
		: m_IsInit(false)
	{

	}

	bool D3D11SamplerState::Init(D3D11_FILTER Filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_COMPARISON_FUNC compare, uint32_t maxAnisotropy, float minLOD, float maxLOD, float mipLODBias)
	{
		if (m_IsInit)
			return false;

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = Filter;
		samplerDesc.AddressU = addressMode;
		samplerDesc.AddressV = addressMode;
		samplerDesc.AddressW = addressMode;
		samplerDesc.MaxAnisotropy = maxAnisotropy;
		samplerDesc.ComparisonFunc = compare;
		samplerDesc.MinLOD = minLOD;
		samplerDesc.MaxLOD = maxLOD;
		samplerDesc.MipLODBias = mipLODBias;

		if (Failed(D3D11Device::GetDev()->CreateSamplerState(&samplerDesc, m_SamplerState.GetPointerAddress())))
			return false;

		m_IsInit = true;

		return true;
	}

	void D3D11SamplerState::Release()
	{
		m_IsInit = false;
		m_SamplerState = nullptr;
	}
};