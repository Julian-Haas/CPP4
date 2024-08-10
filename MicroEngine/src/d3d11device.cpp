//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "d3d11device.h"
#include "d3d11vertexshader.h"
#include "d3d11pixelshader.h"
#include "vertex.h"
#include "color.h"
#include "d3d11buffer.h"

namespace me
{
	constexpr int SHADER_MODEL_MAJOR = 5;
    constexpr int SHADER_MODEL_MINOR = 0;

    constexpr DXGI_FORMAT RT_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr uint32_t RT_BINDFLAGS = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	/*static*/ D3D11Device* D3D11Device::ms_Instance = nullptr;
    /*static*/ std::vector<ID3D11ShaderResourceView*> D3D11Device::s_TmpShaderResourceViewBuffer;
    /*static*/ std::vector<ID3D11RenderTargetView*> D3D11Device::s_TmpRenderTargetViewBuffer;
    /*static*/ std::vector<ID3D11SamplerState*> D3D11Device::s_TmpSamplerStateBuffer;
	/*static*/ std::vector<ID3D11Buffer*> D3D11Device::s_TmpBufferBuffer;

	DXGI_FORMAT g_SwapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	void GetRTDimensions(RenderTargetSize size, int& width, int& height)
	{
		switch(size)
		{
		case RenderTargetSize::Full:
		    break;
		case RenderTargetSize::Half:
			width = width/2;
			height = height/2;
		    break;
		case RenderTargetSize::Quarter:
			width = width/4;
			height = height/4;
		    break;
		case RenderTargetSize::Eighth:
			width = width/8;
			height = height/8;
		    break;
		default:
			ME_ASSERT(false);
			break;
		}
	}

	/*static*/ void D3D11Device::DestroyInstance()
	{
		ME_SAFE_DELETE(ms_Instance);
	}

	/*static*/ D3D11Device* D3D11Device::GetInstance()
	{
		if(!ms_Instance)
			ms_Instance = new D3D11Device();

		return ms_Instance;
	}

	/*static*/ ID3D11Device* D3D11Device::GetDev()
	{
		return GetInstance()->GetDevice();
	}

	/*static*/ ID3D11DeviceContext* D3D11Device::GetContext()
	{
		return GetInstance()->GetImmediateContext();
	}

	bool D3D11Device::Create(HWND hWnd, int width, int height)
	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferCount = 2;
		desc.BufferDesc.Format = g_SwapChainFormat;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = hWnd;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Windowed = true; //TODO: handle exclusive fullscreen mode

		UINT flags = 0;
#ifdef TARGET_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		if (Failed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &desc, m_SwapChain.GetPointerAddress(), m_Device.GetPointerAddress(), 0, m_ImmediateContext.GetPointerAddress())))
			return false;

		if (!CreateBackBuffer())
			return false;

		if (!CreateDepthBuffer())
			return false;

		return true;
	}

	void D3D11Device::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
	    m_ImmediateContext->IASetPrimitiveTopology(topology);
	}

    void D3D11Device::SetViewport(const D3D11_VIEWPORT* viewport)
	{
	    SetViewports(1, viewport);
	}

    void D3D11Device::SetViewports(uint32_t numViewports, const D3D11_VIEWPORT* viewports)
	{
	    m_ImmediateContext->RSSetViewports(numViewports, viewports);
	}

	void D3D11Device::Draw(uint32_t vertexCount, uint32_t startVertexLocation)
	{
	    m_ImmediateContext->Draw(vertexCount, startVertexLocation);		
	}

	void D3D11Device::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation)
	{
	    m_ImmediateContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
	}

	bool D3D11Device::Present(uint32_t syncInterval)
	{
		return !Failed(m_SwapChain->Present(syncInterval, 0));
	}

	void D3D11Device::SetTextureAsShaderResourceVS(uint32_t slot, D3D11Texture2D* texture)
	{
		D3D11Texture2D** textures = &texture;
	    SetTexturesAsShaderResourcesVS(slot, 1, textures);
	}

	void D3D11Device::SetTexturesAsShaderResourcesVS(uint32_t startSlot, uint32_t numTextures, D3D11Texture2D** textures)
	{
		for(uint32_t i = 0; i < numTextures; ++i)
			s_TmpShaderResourceViewBuffer.push_back(textures[i] ? textures[i]->GetSRV() : nullptr);

	    m_ImmediateContext->VSSetShaderResources(startSlot, numTextures, s_TmpShaderResourceViewBuffer.data());
		s_TmpShaderResourceViewBuffer.clear();
	}

	void D3D11Device::SetTextureAsShaderResourcePS(uint32_t slot, D3D11Texture2D* texture)
	{
	    D3D11Texture2D** textures = &texture;
	    SetTexturesAsShaderResourcesPS(slot, 1, textures);
	}

	void D3D11Device::SetTexturesAsShaderResourcesPS(uint32_t startSlot, uint32_t numTextures, D3D11Texture2D** textures)
	{
		for(uint32_t i = 0; i < numTextures; ++i)
			s_TmpShaderResourceViewBuffer.push_back(textures[i] ? textures[i]->GetSRV() : nullptr);

	    m_ImmediateContext->PSSetShaderResources(startSlot, numTextures, s_TmpShaderResourceViewBuffer.data());
		s_TmpShaderResourceViewBuffer.clear();
	}

	void D3D11Device::SetTextureAsRenderTarget(D3D11Texture2D* texture, bool setDSV)
	{
	    D3D11Texture2D** textures = &texture;
	    SetTexturesAsRenderTargets(1, textures, setDSV);
	}

	void D3D11Device::SetTexturesAsRenderTargets(uint32_t numTextures, D3D11Texture2D** textures, bool setDSV)
	{
	    for(uint32_t i = 0; i < numTextures; ++i)
			s_TmpRenderTargetViewBuffer.push_back(textures[i] ? textures[i]->GetRTV() : nullptr);

		m_ImmediateContext->OMSetRenderTargets(numTextures, s_TmpRenderTargetViewBuffer.data(), setDSV ? m_DepthBuffer.GetDSV() : nullptr);
		s_TmpRenderTargetViewBuffer.clear();
	}

	void D3D11Device::ClearBackBuffer(const Color& color)
	{
		ClearRenderTarget(&m_BackBuffer, color);
	}

	void D3D11Device::ClearRenderTarget(D3D11Texture2D* texture, const Color& color)
	{
		ME_ASSERT(texture->GetRTV() != nullptr);
		m_ImmediateContext->ClearRenderTargetView(texture->GetRTV(), color);
	}

	void D3D11Device::ClearDSV(uint8_t clearFlags, float depth, uint8_t stencil)
	{
		m_ImmediateContext->ClearDepthStencilView(m_DepthBuffer.GetDSV(), clearFlags, depth, stencil);
	}

	void D3D11Device::SetSamplerVS(uint32_t slot, D3D11SamplerState* sampler)
	{
		D3D11SamplerState** samplers = &sampler;
	    SetSamplersVS(slot, 1, samplers);
	}

	void D3D11Device::SetSamplersVS(uint32_t startSlot, uint32_t numSamplers, D3D11SamplerState** samplers)
	{
		for(uint32_t i = 0; i < numSamplers; ++i)
			s_TmpSamplerStateBuffer.push_back(samplers[i] ? samplers[i]->GetSamplerState() : nullptr);

		m_ImmediateContext->VSSetSamplers(startSlot, numSamplers, s_TmpSamplerStateBuffer.data());
		s_TmpSamplerStateBuffer.clear();
	}

    void D3D11Device::SetSamplerPS(uint32_t slot, D3D11SamplerState* sampler)
	{
		D3D11SamplerState** samplers = &sampler;
	    SetSamplersPS(slot, 1, samplers);
	}

	void D3D11Device::SetSamplersPS(uint32_t startSlot, uint32_t numSamplers, D3D11SamplerState** samplers)
	{
		for(uint32_t i = 0; i < numSamplers; ++i)
			s_TmpSamplerStateBuffer.push_back(samplers[i] ? samplers[i]->GetSamplerState() : nullptr);

		m_ImmediateContext->PSSetSamplers(startSlot, numSamplers, s_TmpSamplerStateBuffer.data());
		s_TmpSamplerStateBuffer.clear();
	}

	void D3D11Device::SetShader(D3D11VertexShader* shader)
	{
		m_ImmediateContext->VSSetShader(shader->GetShader(), nullptr, 0);
		m_ImmediateContext->IASetInputLayout(shader->GetInputLayout());
        SetConstantBufferVS(2, shader->GetPropertiesBuffer());
	}

	void D3D11Device::SetShader(D3D11PixelShader* shader)
	{
		m_ImmediateContext->PSSetShader(shader->GetShader(), nullptr, 0);
		SetConstantBufferPS(2, shader->GetPropertiesBuffer());
	}

	void D3D11Device::SetConstantBufferVS(uint32_t slot, D3D11Buffer* buffer)
	{
		D3D11Buffer** buffers = &buffer;
	    SetConstantBuffersVS(slot, 1, buffers);
	}

	void D3D11Device::SetConstantBuffersVS(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers)
	{
	    for(uint32_t i = 0; i < numBuffers; ++i)
			s_TmpBufferBuffer.push_back(buffers[i] ? buffers[i]->GetBuffer() : nullptr);

		m_ImmediateContext->VSSetConstantBuffers(startSlot, numBuffers, s_TmpBufferBuffer.data());
		s_TmpBufferBuffer.clear();
	}

	void D3D11Device::SetConstantBufferPS(uint32_t slot, D3D11Buffer* buffer)
	{
	    D3D11Buffer** buffers = &buffer;
	    SetConstantBuffersPS(slot, 1, buffers);
	}

	void D3D11Device::SetConstantBuffersPS(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers)
	{
	    for(uint32_t i = 0; i < numBuffers; ++i)
			s_TmpBufferBuffer.push_back(buffers[i] ? buffers[i]->GetBuffer() : nullptr);

		m_ImmediateContext->PSSetConstantBuffers(startSlot, numBuffers, s_TmpBufferBuffer.data());
		s_TmpBufferBuffer.clear();
	}

	void D3D11Device::SetVertexBuffer(uint32_t slot, D3D11Buffer* buffer, uint32_t stride, uint32_t offset)
	{
	    D3D11Buffer** buffers = &buffer;
	    SetVertexBuffers(slot, 1, buffers, &stride, &offset);
	}

	void D3D11Device::SetVertexBuffers(uint32_t startSlot, uint32_t numBuffers, D3D11Buffer** buffers, uint32_t* strides, uint32_t* offsets)
	{
	    for(uint32_t i = 0; i < numBuffers; ++i)
			s_TmpBufferBuffer.push_back(buffers[i] ? buffers[i]->GetBuffer() : nullptr);

		m_ImmediateContext->IASetVertexBuffers(startSlot, numBuffers, s_TmpBufferBuffer.data(), strides, offsets);
		s_TmpBufferBuffer.clear();
	}

	void D3D11Device::SetIndexBuffer(D3D11Buffer* buffer, DXGI_FORMAT format, uint32_t offset)
	{
	    m_ImmediateContext->IASetIndexBuffer(buffer->GetBuffer(), format, offset);
	}

	bool D3D11Device::CreateBackBuffer()
	{
		ID3D11Texture2D* backBuffer = nullptr;
		if (Failed(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer))))
			return false;

		if (!m_BackBuffer.InitFromExisting(backBuffer, D3D11_BIND_RENDER_TARGET))
			return false;

		return true;
	}

	bool D3D11Device::CreateDepthBuffer()
	{
		if (!m_DepthBuffer.Create(m_BackBuffer.GetWidth(), m_BackBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL))
			return false;

		return true;
	}

	void D3D11Device::ResizeSwapchain(int width, int height)
	{
		m_DepthBuffer.Release();
		m_BackBuffer.Release();

		if (Failed(m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
			return;

		CreateBackBuffer();
		CreateDepthBuffer();

		for(auto& kvp : m_RenderTargetPool)
		{
			int w = width;
			int h = height;
			GetRTDimensions(kvp.first, w, h);
		    for(auto& rt : kvp.second)
		    {
		        rt->Release();
				rt->Create(w, h, RT_FORMAT, RT_BINDFLAGS);
		    }
		}


	}

	D3D11Texture2D* D3D11Device::FindTexture(const std::string& name)
	{
        const std::string path = ResolveURI(name);
		D3D11Texture2D& texture = m_Textures[path];
		if (!texture.IsInit())
		{
			if(StartsWith(path, "me://textures/"))
			{
				std::string color = path.substr(strlen("me://textures/"));
				texture.Create(color);
			}
			else
			{
			    texture.CreateFromFile(path);
			}
		}
		return &texture;
	}

	D3D11VertexShader* D3D11Device::FindVertexShader(const std::string& name)
	{
        const std::string path = ResolveURI(name);
		D3D11VertexShader& shader = m_VertexShaders[path];
		ME_ASSERT(shader.IsInit());
		return &shader;
	}

	bool D3D11Device::AddVertexShader(const std::string& name, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes, int numTextures)
	{
        const std::string path = ResolveURI(name);
		D3D11VertexShader& shader = m_VertexShaders[path];
		ME_ASSERT(!shader.IsInit());

		if (EndsWith(path, ".cso"))
			return shader.CreateFromPrecompiled(path, vertexDescription, settingsBufferSizeBytes, numTextures);

		if (EndsWith(path, ".hlsl"))
			return shader.CreateFromSource(path, "VS_Main", SHADER_MODEL_MAJOR, SHADER_MODEL_MINOR, vertexDescription, settingsBufferSizeBytes, numTextures);

	    ME_ASSERT(false);
		return false;
	}

	bool D3D11Device::AddVertexShaderSource(const void* src, std::size_t size, const std::string& name, const VertexDescription& vertexDescription, size_t settingsBufferSizeBytes, int numTextures)
	{
	    D3D11VertexShader& shader = m_VertexShaders[name];
		return shader.CreateFromSource(src, size, name, "VS_Main", SHADER_MODEL_MAJOR, SHADER_MODEL_MINOR, vertexDescription, settingsBufferSizeBytes, numTextures);
	}

	D3D11PixelShader* D3D11Device::FindPixelShader(const std::string& name)
	{
        const std::string path = ResolveURI(name);
		D3D11PixelShader& shader = m_PixelShaders[path];
		ME_ASSERT(shader.IsInit());
		return &shader;
	}

	bool D3D11Device::AddPixelShader(const std::string& name, size_t settingsBufferSizeBytes, int numTextures)
	{
        const std::string path = ResolveURI(name);
		D3D11PixelShader& shader = m_PixelShaders[path];
		ME_ASSERT(!shader.IsInit());

		if (EndsWith(path, ".cso"))
			return shader.CreateFromPrecompiled(path, settingsBufferSizeBytes, numTextures);
		if (EndsWith(path, ".hlsl"))
			return shader.CreateFromSource(path, "PS_Main", SHADER_MODEL_MAJOR, SHADER_MODEL_MINOR, settingsBufferSizeBytes, numTextures);

		ME_ASSERT(false);
		return false;
	}

	bool D3D11Device::AddPixelShaderSource(const void* src, std::size_t size, const std::string& name, size_t settingsBufferSizeBytes, int numTextures)
	{
	    D3D11PixelShader& shader = m_PixelShaders[name];
		return shader.CreateFromSource(src, size, name, "PS_Main", SHADER_MODEL_MAJOR, SHADER_MODEL_MINOR, settingsBufferSizeBytes, numTextures);
	}

	std::unique_ptr<D3D11Texture2D> D3D11Device::AcquireRenderTarget(RenderTargetSize size)
	{
		auto& pool = m_RenderTargetPool[size];
		if (!pool.empty())
		{
			auto rt = std::move(*(pool.end()-1));
			pool.erase(pool.end()-1);
		    return rt;
		}

		std::unique_ptr<D3D11Texture2D> rt = std::make_unique<D3D11Texture2D>();
		int width = m_BackBuffer.GetWidth();
		int height = m_BackBuffer.GetHeight();
		GetRTDimensions(size, width, height);
		rt->Create(width, height, RT_FORMAT, RT_BINDFLAGS);
		return rt;
	}

	void D3D11Device::ReleaseRenderTarget(RenderTargetSize size, std::unique_ptr<D3D11Texture2D>& rt)
	{
		int width = m_BackBuffer.GetWidth();
		int height = m_BackBuffer.GetHeight();
		GetRTDimensions(size, width, height);

		if(rt->GetWidth() != width || rt->GetHeight() != height)
		{
		    rt->Release();
			rt->Create(width, height, RT_FORMAT, RT_BINDFLAGS);
		}

	    m_RenderTargetPool[size].push_back(std::move(rt));
	}
};