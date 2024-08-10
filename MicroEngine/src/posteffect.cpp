#include "stdafx.h"

#include "posteffect.h"

#include "d3d11device.h"
#include "d3d11samplerstate.h"
#include "d3d11pixelshader.h"
#include "vertex.h"

namespace me
{
	//It's actually not a quad but only one triangle for optimization
	//{ 1.0f,	-1.0f, 0.0f, 1.0f, 1.0f },
	//{ -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
	//{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
	//{ -1.0f, 1.0f, 0.0f, 0.0f, 0.0f }
    VertexTextured g_QuadVertices[] =
	{   //  X Y Z U V
        {-1.0f, -1.0f, 0.0f, 0.0f, 2.0f},
        {-1.0f, 3.0f, 0.0f, 0.0f, 0.0f},
        {3.0f, -1.0f, 0.0f, 2.0f, 2.0f}
	};

    const char* g_VertexShader =
        "struct VS_INPUT_UV { float4 Position : POSITION0; float2 UV : TEXCOORD0; };\
        struct POSTEFFECT_PARAM { float4 Position : SV_POSITION; float2 UV : TEXCOORD0; };\
        POSTEFFECT_PARAM VS_Main(VS_INPUT_UV IN)\
        {\
            POSTEFFECT_PARAM OUT = (POSTEFFECT_PARAM)0;\
            OUT.Position = float4(IN.Position.xyz, 1);\
            OUT.UV = IN.UV;\
            return OUT;\
        }";

    Mesh PostEffect::s_PostEffectMesh(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, VertexTextured::s_Description);
	D3D11SamplerState PostEffect::s_PostEffectSampler;
	size_t PostEffect::s_PostEffectRefs = 0;

    PostEffect::PostEffect(PostEffectPass pass, std::string pixelShader)
        : m_Pass(pass)
        , m_PixelShader(pixelShader)
        , m_ParamsDirty(true)
    {
        if(s_PostEffectRefs == 0)
        {
            D3D11Device::GetInstance()->AddVertexShaderSource(g_VertexShader, strlen(g_VertexShader), "me://vertexshader/fullscreenquad.hlsl", s_PostEffectMesh.GetVertexDescription());
            s_PostEffectMesh.AddVertices(g_QuadVertices, ME_ARRAY_COUNT(g_QuadVertices));
		    s_PostEffectMesh.CreateBuffers();
		    s_PostEffectSampler.Init(D3D11SamplerState::ANISOTROPIC_FILTER, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 4, 0, D3D11_FLOAT32_MAX, 0);
        }
        ++s_PostEffectRefs;
    }

    PostEffect::~PostEffect()
    {
        --s_PostEffectRefs;
		if (s_PostEffectRefs == 0)
		{
			s_PostEffectMesh.ReleaseBuffers();
            s_PostEffectSampler.Release();
		}
    }

    void PostEffect::Render(D3D11Texture2D* source, D3D11Texture2D* target)
    {
        D3D11Device* meDevice = D3D11Device::GetInstance();
        D3D11PixelShader* pixelShader = meDevice->FindPixelShader(m_PixelShader);
        if(m_ParamsDirty)
        {
            WriteParams(*pixelShader->GetPropertiesBuffer());
            m_ParamsDirty = false;
        }

        meDevice->SetTextureAsRenderTarget(target, false);
        meDevice->SetTextureAsShaderResourcePS(0, source);

        D3D11VertexShader* vertexShader = meDevice->FindVertexShader("me://vertexshader/fullscreenquad.hlsl");
        meDevice->SetShader(vertexShader);
        meDevice->SetShader(pixelShader);
        meDevice->SetSamplerPS(0, &s_PostEffectSampler);
        s_PostEffectMesh.Set(0);

        D3D11_VIEWPORT viewPort = {};
        viewPort.Width = static_cast<float>(target->GetWidth());
        viewPort.Height = static_cast<float>(target->GetHeight());
        viewPort.TopLeftX = 0;
        viewPort.TopLeftY = 0;
        viewPort.MinDepth = 0.0f;
        viewPort.MaxDepth = 1.0f;
	    meDevice->SetViewport(&viewPort);

        meDevice->GetImmediateContext()->Draw(static_cast<UINT>(s_PostEffectMesh.GetNumVertices()), 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct DepthOfFieldCBData
    {
        float m_FocusDistance;
        float m_FocusRange;
        float m_Unused[2];
    } g_DepthOfFieldCBData = {};

    DepthOfFieldPostEffect::DepthOfFieldPostEffect()
        : PostEffect(Pass, "assets://DepthOfFieldPostEffect.hlsl")
        , m_FocusDistance(0.5f)
        , m_FocusRange(0.1f)
    {
        D3D11Device::GetInstance()->AddPixelShader(GetPixelShader(), sizeof(DepthOfFieldCBData), 1);
    }

    void DepthOfFieldPostEffect::WriteParams(D3D11Buffer& buffer)
    {
        g_DepthOfFieldCBData.m_FocusDistance = m_FocusDistance;
        g_DepthOfFieldCBData.m_FocusRange = m_FocusRange;
        buffer.Write(&g_DepthOfFieldCBData, sizeof(DepthOfFieldCBData));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct FinalCBData
    {
        float m_Brightness;
        float m_Unused[3];
    } g_FinalCBData = {};

    FinalPostEffect::FinalPostEffect()
        : PostEffect(Pass, "assets://FinalPostEffect.hlsl")
        , m_Brightness(1.0f)
    {
        D3D11Device::GetInstance()->AddPixelShader(GetPixelShader(), sizeof(FinalCBData), 1);
    }

    void FinalPostEffect::WriteParams(D3D11Buffer& buffer)
    {
        g_FinalCBData.m_Brightness = m_Brightness;
        buffer.Write(&g_FinalCBData, sizeof(FinalCBData));
    }
}

