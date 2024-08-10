//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "ShaderCommon.hlsl"

cbuffer PerModelCB : register(b1)
{
	float4x4 g_ModelMatrix;
}

SamplerState g_Sampler : register(s0);
Texture2D g_DiffuseTexture : register(t0);

// Vertex Shader
PS_INPUT_MESH VS_Main(VS_INPUT_NORMAL_UV IN)
{
	PS_INPUT_MESH OUT = (PS_INPUT_MESH)0;

	float4x4 MVP = mul(g_ModelMatrix, g_ViewProjection);
	OUT.Position = mul(IN.Position, MVP);
	OUT.Normal = normalize(mul(float4(IN.Normal, 0), g_ModelMatrix)).xyz;
	OUT.UV = IN.UV;
	
	float4 worldPos = mul(IN.Position, g_ModelMatrix);	
	OUT.InvViewVec = normalize((g_CameraPos - worldPos).xyz);
	OUT.LinearZ = (OUT.Position.z - g_NearPlane) / (g_FarPlane - g_NearPlane);
	OUT.WorldPos = worldPos.xyz;
	return OUT;
}

// Pixel Shader
float4 PS_Main(PS_INPUT_MESH IN) : SV_TARGET
{
	float4 color = g_DiffuseTexture.Sample(g_Sampler, IN.UV);
	color.a = IN.LinearZ;
	return color;
}