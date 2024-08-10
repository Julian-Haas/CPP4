//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

struct LightData
{
	float4 m_Position;
	float3 m_Direction;
	float m_Range;
	float4 m_Color;
	int m_Type;
	float m_Intensity;
	float m_SpotAngleInner;
	float m_SpotAngleOuter;
};

cbuffer PerFrameCB : register(b0)
{
	float4x4 g_ViewProjection;
	float4 g_CameraPos;
	float g_DeltaTime;
	float g_TotalTime;
	float g_NearPlane;
	float g_FarPlane;
    LightData g_Lights[4];
};

struct VS_INPUT_UV
{
	float4 Position : POSITION0;
	float2 UV : TEXCOORD0;
};

struct VS_INPUT_NORMAL_UV
{
	float4 Position : POSITION0;
	float3 Normal: NORMAL0;
	float2 UV : TEXCOORD0;
};

struct PS_INPUT_MESH
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL0;
	float2 UV : TEXCOORD0;
	float3 InvViewVec : TEXCOORD1;
	float LinearZ : TEXCOORD2;
	float3 WorldPos : TEXCOORD3;
};

struct PS_INPUT_POSTEFFECT_PARAM
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
};

// Blurs using a 5x5 filter kernel
float4 BlurFunction5x5(Texture2D tex, SamplerState samplerState, float2 uv)
{
	float width, height;
	tex.GetDimensions(width, height);

    return (
        tex.Sample(samplerState, uv + float2(-2.0f / width, -2.0f / height)) +
        tex.Sample(samplerState, uv + float2(-1.0f / width, -2.0f / height)) +
        tex.Sample(samplerState, uv + float2(0,             -2.0f / height)) +
        tex.Sample(samplerState, uv + float2(1.0f / width,  -2.0f / height)) +
        tex.Sample(samplerState, uv + float2(2.0f / width,  -2.0f / height)) +

        tex.Sample(samplerState, uv + float2(-2.0f / width, -1.0f / height)) +
        tex.Sample(samplerState, uv + float2(-1.0f / width, -1.0f / height)) +
        tex.Sample(samplerState, uv + float2(0,             -1.0f / height)) +
        tex.Sample(samplerState, uv + float2(1.0f / width,  -1.0f / height)) +
        tex.Sample(samplerState, uv + float2(2.0f / width,  -1.0f / height)) +
				   
        tex.Sample(samplerState, uv + float2(-2.0f / width, 0)) +
        tex.Sample(samplerState, uv + float2(-1.0f / width, 0)) +
        tex.Sample(samplerState, uv + float2(0,             0)) +
        tex.Sample(samplerState, uv + float2(1.0f / width,  0)) +
        tex.Sample(samplerState, uv + float2(2.0f / width,  0)) +
				   
        tex.Sample(samplerState, uv + float2(-2.0f / width, 1.0f / height)) +
        tex.Sample(samplerState, uv + float2(-1.0f / width, 1.0f / height)) +
        tex.Sample(samplerState, uv + float2(0,             1.0f / height)) +
        tex.Sample(samplerState, uv + float2(1.0f / width,  1.0f / height)) +
        tex.Sample(samplerState, uv + float2(2.0f / width,  1.0f / height)) +
					   
        tex.Sample(samplerState, uv + float2(-2.0f / width, 2.0f / height)) +
        tex.Sample(samplerState, uv + float2(-1.0f / width, 2.0f / height)) +
        tex.Sample(samplerState, uv + float2(0,             2.0f / height)) +
        tex.Sample(samplerState, uv + float2(1.0f / width,  2.0f / height)) +
        tex.Sample(samplerState, uv + float2(2.0f / width,  2.0f / height))
    ) / 25;
}