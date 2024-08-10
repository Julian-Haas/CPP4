//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include "comptr.h"
#include "d3d11samplerstate.h"
#include "d3d11texture2d.h"
#include "util.h"
#include "me_interface.h"

namespace me
{
    class D3D11Buffer;
    class Mesh;
    class D3D11VertexShader;
	class D3D11PixelShader;
	struct Color;
	struct VertexDescription;

	enum class RenderTargetSize
	{
	    Full,
		Half,
		Quarter,
		Eighth
	};

	class D3D11Device
	{
	public:
		ME_API static D3D11Device* GetInstance();
		ME_API static ID3D11Device* GetDev();
		ME_API static ID3D11DeviceContext* GetContext();
		ME_API static void DestroyInstance();

		ME_API bool Create(HWND hWnd, int width, int height);

		ME_API void ResizeSwapchain(int width, int height);
		ME_API int GetSwapchainWidth() const { return m_BackBuffer.m_Width; }
		ME_API int GetSwapchainHeight() const { return m_BackBuffer.m_Height; }
		D3D11Texture2D* GetBackBuffer() { return &m_BackBuffer; }

		ID3D11Device* GetDevice() { return m_Device; }
		ID3D11DeviceContext* GetImmediateContext() { return m_ImmediateContext; }

		ME_API void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
		ME_API void SetViewport(const D3D11_VIEWPORT* viewport);
		ME_API void SetViewports(uint32_t numViewports, const D3D11_VIEWPORT* viewports);

		ME_API void Draw(uint32_t vertexCount, uint32_t startVertexLocation);
		ME_API void DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation = 0);
	    ME_API bool Present(uint32_t syncInterval);

		//Textures
	    ME_API void SetTextureAsShaderResourceVS(uint32_t slot, D3D11Texture2D* texture);
		ME_API void SetTexturesAsShaderResourcesVS(uint32_t startSlot, uint32_t numTextures, D3D11Texture2D** textures);
		ME_API void SetTextureAsShaderResourcePS(uint32_t slot, D3D11Texture2D* texture);
		ME_API void SetTexturesAsShaderResourcesPS(uint32_t startSlot, uint32_t numTextures, D3D11Texture2D** textures);
		ME_API void SetTextureAsRenderTarget(D3D11Texture2D* texture, bool setDSV);
		ME_API void SetTexturesAsRenderTargets(uint32_t numTextures, D3D11Texture2D** textures, bool setDSV);
		ME_API void ClearBackBuffer(const Color& color);
		ME_API void ClearRenderTarget(D3D11Texture2D* texture, const Color& color);
		ME_API void ClearDSV(uint8_t clearFlags, float depth, uint8_t stencil);

		//Samplers
		ME_API void SetSamplerVS(uint32_t slot, D3D11SamplerState* sampler);
		ME_API void SetSamplersVS(uint32_t startSlot, uint32_t numSamplers, D3D11SamplerState** samplers);
		ME_API void SetSamplerPS(uint32_t slot, D3D11SamplerState* sampler);
		ME_API void SetSamplersPS(uint32_t startSlot, uint32_t numSamplers, D3D11SamplerState** samplers);

		//Shaders
		ME_API void SetShader(D3D11VertexShader* shader);
		ME_API void SetShader(D3D11PixelShader* shader);

		//Buffers
		ME_API void SetConstantBufferVS(uint32_t slot, D3D11Buffer* buffer);
		ME_API void SetConstantBuffersVS(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers);
		ME_API void SetConstantBufferPS(uint32_t slot, D3D11Buffer* buffer);
		ME_API void SetConstantBuffersPS(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers);
		ME_API void SetVertexBuffer(uint32_t slot, D3D11Buffer* buffer, uint32_t stride, uint32_t offset);
		ME_API void SetVertexBuffers(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers, uint32_t* strides, uint32_t* offsets);
		ME_API void SetIndexBuffer(D3D11Buffer* buffer, DXGI_FORMAT format, uint32_t offset);

		//Resource management
		ME_API D3D11Texture2D* FindTexture(const std::string& name);

	    ME_API D3D11VertexShader* FindVertexShader(const std::string& name);
		ME_API bool AddVertexShader(const std::string& name, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes = SHADER_NO_SETTINGS_BUFFER, int numTextures = 0);
		ME_API bool AddVertexShaderSource(const void* src, std::size_t size, const std::string& name, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes = SHADER_NO_SETTINGS_BUFFER, int numTextures = 0);

	    ME_API D3D11PixelShader* FindPixelShader(const std::string& name);
		ME_API bool AddPixelShader(const std::string& name, size_t settingsBufferSizeBytes, int numTextures);
		ME_API bool AddPixelShaderSource(const void* src, std::size_t size, const std::string& name, size_t settingsBufferSizeBytes, int numTextures);

		ME_API std::unique_ptr<D3D11Texture2D> AcquireRenderTarget(RenderTargetSize size);
		ME_API void ReleaseRenderTarget(RenderTargetSize size, std::unique_ptr<D3D11Texture2D>& rt);

	private:
		ME_MOVE_COPY_NOT_ALLOWED(D3D11Device);

		D3D11Device() = default;
		~D3D11Device() = default;

		bool CreateBackBuffer();
		bool CreateDepthBuffer();

		static D3D11Device* ms_Instance;

		D3D11Texture2D m_BackBuffer;
		D3D11Texture2D m_DepthBuffer;
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_ImmediateContext;
		ComPtr<IDXGISwapChain> m_SwapChain;

		std::map<std::string, D3D11Texture2D> m_Textures;
		std::map<std::string, D3D11VertexShader> m_VertexShaders;
		std::map <std::string, D3D11PixelShader> m_PixelShaders;

		std::map<RenderTargetSize, std::vector<std::unique_ptr<D3D11Texture2D>>> m_RenderTargetPool;

		static std::vector<ID3D11ShaderResourceView*> s_TmpShaderResourceViewBuffer;
		static std::vector<ID3D11RenderTargetView*> s_TmpRenderTargetViewBuffer;
		static std::vector<ID3D11SamplerState*> s_TmpSamplerStateBuffer;
		static std::vector<ID3D11Buffer*> s_TmpBufferBuffer;
	};

};