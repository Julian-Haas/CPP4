//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "ShaderCommon.hlsl"

SamplerState g_Sampler : register(s0);
Texture2D g_OffscreenTexture : register(t0);

cbuffer PostEffectCB : register(b2)
{
	float g_Brightness;
	float3 unused;
};

float4 PS_Main(PS_INPUT_POSTEFFECT_PARAM IN) : SV_TARGET
{
	float4 color = g_OffscreenTexture.Sample(g_Sampler, IN.UV);
	return saturate(color * g_Brightness);
}