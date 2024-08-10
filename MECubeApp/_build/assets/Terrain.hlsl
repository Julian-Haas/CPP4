//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "ShaderCommon.hlsl"

cbuffer PerModelCB : register(b1)
{
	float4x4 g_ModelMatrix;
}

Texture2D g_DisplacementTexture : register(t0);

float LookupHeight(int3 location)
{
	float h = g_DisplacementTexture.Load(location).r;
	return pow(h, 2) * 10;
}

float3 CalcNormal(int3 center)
{
	float offset = 1;
	float top = LookupHeight(center + int3(0, offset, 0));
	float right = LookupHeight(center + int3(offset, 0, 0));
	float bottom = LookupHeight(center + int3(0, -offset, 0));
	float left = LookupHeight(center + int3(-offset, 0, 0));

	float heightScale = 3;
	float3 vtop = float3(0, top*heightScale, 1 );
	float3 vright = float3( 1, right*heightScale, 0 );
	float3 vbottom = float3( 0, bottom*heightScale, -1 );
	float3 vleft = float3( -1, left*heightScale, 0 );
	float3 average_n = (cross(vtop, vright) + cross(vright, vbottom) + cross(vbottom, vleft) + cross(vleft, vtop)) / 4;
	return normalize(average_n);
}

// Vertex Shader
PS_INPUT_MESH VS_Main(VS_INPUT_UV IN)
{
	PS_INPUT_MESH OUT = (PS_INPUT_MESH)0;

	float4 position = IN.Position;
	float2 textureSize;
	g_DisplacementTexture.GetDimensions(textureSize.x, textureSize.y);
	int3 center = int3((IN.UV * textureSize).xy, 0);
	position.y = LookupHeight(center);
	float4x4 MVP = mul(g_ModelMatrix, g_ViewProjection);
	OUT.Position = mul(position, MVP);
	float3 normal = CalcNormal(center);
	OUT.Normal = normalize(mul(float4(normal, 0), g_ModelMatrix).xyz);
	OUT.UV = IN.UV;

	float4 worldPos = mul(position, g_ModelMatrix);	
	OUT.InvViewVec = normalize((g_CameraPos - worldPos).xyz);
	OUT.LinearZ = (OUT.Position.z - g_NearPlane) / (g_FarPlane - g_NearPlane);
	OUT.WorldPos = worldPos.xyz;

	return OUT;
}