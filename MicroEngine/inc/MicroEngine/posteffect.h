#pragma once
#include <string>

#include "d3d11buffer.h"
#include "d3d11samplerstate.h"
#include "mesh.h"

namespace me
{
    class D3D11Texture2D;
    typedef int32_t PostEffectPass;

    namespace EnginePostEffectPass
	{
		enum Enum : PostEffectPass
		{
            DepthOfField = 100,
			Final = INT32_MAX
		};
	};

    class PostEffect
    {
    public:
        PostEffect(PostEffectPass pass, std::string pixelShader);
        virtual ~PostEffect();

        PostEffectPass GetPass() const { return m_Pass; }
        const std::string& GetPixelShader() const { return m_PixelShader; }

        virtual void Render(D3D11Texture2D* source, D3D11Texture2D* target);

    protected:
        void SetParamsDirty() { m_ParamsDirty = true; }
        virtual void WriteParams(D3D11Buffer& paramsCB) = 0;

    private:
        ME_MOVE_COPY_NOT_ALLOWED(PostEffect);
        static Mesh s_PostEffectMesh;
		static D3D11SamplerState s_PostEffectSampler;
        static size_t s_PostEffectRefs;

        PostEffectPass m_Pass;
        std::string m_PixelShader;
        bool m_ParamsDirty;
    };

    class DepthOfFieldPostEffect : public PostEffect
    {
    public:
        static const PostEffectPass Pass = EnginePostEffectPass::DepthOfField;

        DepthOfFieldPostEffect();
        virtual ~DepthOfFieldPostEffect() = default;
        float GetFocusDistance() const { return m_FocusDistance; }
        void SetFocusDistance(float value) { m_FocusDistance = value; SetParamsDirty(); }
        float GetFocusRange() const { return m_FocusRange; }
        void SetFocusRange(float value) { m_FocusRange = value; SetParamsDirty(); }
    protected:
        virtual void WriteParams(D3D11Buffer& paramsCB) override;

    private:
        float m_FocusDistance;
        float m_FocusRange;
    };

    class FinalPostEffect : public PostEffect
    {
    public:
        static const PostEffectPass Pass = EnginePostEffectPass::Final;

        FinalPostEffect();
        virtual ~FinalPostEffect() = default;
        float GetBrightness() const { return m_Brightness; }
        void SetBrightness(float value) { m_Brightness = value; SetParamsDirty(); }
    protected:
        virtual void WriteParams(D3D11Buffer& paramsCB) override;

    private:
        float m_Brightness;
    };
};