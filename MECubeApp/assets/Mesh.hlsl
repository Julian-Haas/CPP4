//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "ShaderCommon.hlsl"

cbuffer PerModelCB : register(b1)
{
	float4x4 g_ModelMatrix;
}

cbuffer PerMaterialCB : register(b2)
{
	float4 g_MaterialDiffuse;
	float4 g_MaterialEmissive;
	float4 g_MaterialSpecular;
	float g_MaterialShininess;
	float3 g_Unused;
}

SamplerState g_Sampler : register(s0);
Texture2D g_DiffuseTexture : register(t0);

float CalculateDistanceAttenuation(float3 lightVec, LightData lightData)
{
	float distance = length(lightVec);
	return 1/(pow((distance/lightData.m_Range)*5, 2) + 1);
}

float CalculateSpotLightAttenuation(float3 lightVec, float3 lightDir, float cosTheta, float cosPhi)
{
	float3 nLightVec = normalize(lightVec);
	float rho = dot(nLightVec, -lightDir);
	float thetaMinusPhi = cosTheta - cosPhi; //preshader
	return saturate((rho - cosPhi) / thetaMinusPhi);
}

static const int LIGHT_TYPE_INVALID = 0;
static const int LIGHT_TYPE_DIRECTIONAL = 1;
static const int LIGHT_TYPE_POINT = 2;
static const int LIGHT_TYPE_SPOT = 3;


float4 CalculateBlinnPhong(float4 textureColor, float3 normal, float3 worldPos, float3 invViewDirection)
{
	float4 diffuseLight;
	float4 specularLight;

	for(int i = 0; i < 4; ++i)
	{
		LightData lightData = g_Lights[i];
		if (lightData.m_Type == LIGHT_TYPE_INVALID)
			break;

		float3 lightDirection = normalize(lightData.m_Direction.xyz);
	    float attenuation = 1;

		if (lightData.m_Type == LIGHT_TYPE_POINT)
		{
			float3 lightVec = worldPos - lightData.m_Position.xyz;
			lightDirection = normalize(lightVec);
	        attenuation = CalculateDistanceAttenuation(lightVec, lightData);
		}
		else if (lightData.m_Type == LIGHT_TYPE_SPOT)
		{
		    float3 lightVec = lightData.m_Position.xyz - worldPos;
			attenuation = CalculateSpotLightAttenuation(lightVec, lightDirection, lightData.m_SpotAngleInner, lightData.m_SpotAngleOuter) * CalculateDistanceAttenuation(lightVec, lightData);
		}

	    //Diffuse
	    float diffuseIntensity = saturate(dot(normal, -lightDirection));
	    diffuseLight += lightData.m_Color * diffuseIntensity * attenuation * lightData.m_Intensity;

	    //Specular
	    float3 reflection = normalize(reflect(lightDirection, normal));
	    float specularIntensity = saturate(dot(reflection, invViewDirection));
	    specularLight += pow(specularIntensity, g_MaterialShininess) * attenuation * lightData.m_Intensity;
	}

	//Ambient
	float ambientIntensity = 0.0f;
	float4 ambient = float4(ambientIntensity, ambientIntensity, ambientIntensity, 1);

	float4 color = (textureColor * saturate(ambient + g_MaterialEmissive + g_MaterialDiffuse * diffuseLight) + g_MaterialSpecular * specularLight);
	return color;
}

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
	float3 normal = normalize(IN.Normal);
	float4 texColor = g_DiffuseTexture.Sample(g_Sampler, IN.UV);
	float3 invViewVec = normalize(IN.InvViewVec);
	float4 color = CalculateBlinnPhong(texColor, normal, IN.WorldPos, invViewVec);
	color.a = IN.LinearZ;
	return color;
}