//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "ShaderCommon.hlsl"

SamplerState g_Sampler : register(s0);
Texture2D g_OffscreenTexture : register(t0);

cbuffer PostEffectCB : register(b2)
{
	float g_FocusDistance;
	float g_FocusRange;
	float2 unused;
};

float4 PS_Main(PS_INPUT_POSTEFFECT_PARAM IN) : SV_TARGET
{
	float4 color = g_OffscreenTexture.Sample(g_Sampler, IN.UV);
	float depth = color.a * 5;
    float4 blur = BlurFunction5x5(g_OffscreenTexture, g_Sampler, IN.UV);
	float blurFactor = clamp((depth - g_FocusDistance)/g_FocusRange, -1, 1);
	//visualize blur factor
    //if(blurFactor < 0)
    //    return blurFactor * -float4(1, 0, 0, 1);
	//return blurFactor;
	float4 result = lerp(color, blur, blurFactor);
	result.a = color.a; //preserve linear z
    return result;
}