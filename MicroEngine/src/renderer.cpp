//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "renderer.h"
#include "entity.h"
#include "renderercomponent.h"
#include "cameracomponent.h"
#include "d3d11device.h"
#include "color.h"
#include "vertex.h"
#include "d3d11vertexshader.h"
#include "d3d11pixelshader.h"
#include "lightcomponent.h"
#include "memath.h"
#include "transformcomponent.h"
#include "window.h"

namespace me
{
	constexpr int MAX_NUM_LIGHTS = 4;

	struct PerFrameCBData
	{
		DirectX::XMMATRIX m_ViewProjectionMatrix;
		DirectX::XMVECTOR m_CameraPos;
		float m_DeltaTime;
		float m_TotalTime;
		float m_NearPlane;
		float m_FarPlane;
		LightData m_Lights[MAX_NUM_LIGHTS];
	} g_PerFrameCBData = {};

	Renderer::Renderer()
	    : m_IsInit(false)
	{
		EventBus::GetInstance()->Register(WindowSizeChangedEvent::ID, reinterpret_cast<size_t>(this), [this](const std::shared_ptr<IEvent>& event)
		{
            const auto ev = std::static_pointer_cast<WindowSizeChangedEvent>(event);
			OnWindowResize(ev->m_Width, ev->m_Height, ev->m_Minimized);
		});
	}

	Renderer::~Renderer()
	{
		EventBus::GetInstance()->Unregister(WindowSizeChangedEvent::ID, reinterpret_cast<size_t>(this));
		if (m_IsInit)
		{
			me::D3D11Device::DestroyInstance();
		}
	}

	bool Renderer::Init(HWND hWnd, int width, int height)
	{
		if (m_IsInit)
			return false;

		D3D11Device* meDevice = D3D11Device::GetInstance();
		if (!meDevice->Create(hWnd, width, height))
			return false;

		if (!m_PerFrameCB.CreateDynamic(sizeof(PerFrameCBData), D3D11_BIND_CONSTANT_BUFFER))
			return false;

		for (int i = 0; i < 2; ++i)
		    m_OffscreenRenderTargets[i] = meDevice->AcquireRenderTarget(RenderTargetSize::Full);

		AddPostEffect(std::make_unique<DepthOfFieldPostEffect>());
		AddPostEffect(std::make_unique<FinalPostEffect>());

		m_IsInit = true;
		return true;
	}

    void Renderer::AddPostEffect(std::unique_ptr<PostEffect> postEffect)
	{
		m_PostEffects.insert(std::make_pair(postEffect->GetPass(), std::move(postEffect)));
	}

	const std::vector<std::vector<ComponentID>>& Renderer::GetRequiredComponents() const
	{
		static std::vector<std::vector<ComponentID>> s_Required = {{RendererComponent::ID}, {CameraComponent::ID}, {LightComponent::ID}};
		return s_Required;
	}

	void Renderer::OnAddEntity(const std::shared_ptr<Entity>& entity)
	{
		auto rendererCmp = entity->GetComponent<RendererComponent>().lock();
        const auto cameraCmp = entity->GetComponent<CameraComponent>().lock();
        const auto lightCmp = entity->GetComponent<LightComponent>().lock();

		ME_ASSERT(rendererCmp || cameraCmp || lightCmp);

		if(rendererCmp)
		    m_RootNode.AddChild(rendererCmp->GetSceneNode());

		if(cameraCmp)
			m_Cameras[entity->GetID()] = entity;

		if(lightCmp)
			m_Lights[entity->GetID()] = entity;
	}
	
	void Renderer::OnRemoveEntity(EntityID entityID)
	{
		m_RootNode.RemoveChild(entityID);
		m_Cameras.erase(entityID);
		m_Lights.erase(entityID);
	}

	void Renderer::Update(float deltaTime)
	{
		D3D11Device* meDevice = D3D11Device::GetInstance();
		meDevice->ClearBackBuffer(Color{ 1.0f, 0.0f, 1.0f, 1.0f });
		meDevice->ClearDSV(D3D11_CLEAR_DEPTH, 1.0f, 0);

		g_PerFrameCBData.m_DeltaTime = deltaTime;
		g_PerFrameCBData.m_TotalTime += deltaTime;

		memset(g_PerFrameCBData.m_Lights, 0, sizeof(g_PerFrameCBData.m_Lights));
		int lightIndex = 0;
		for(auto& kvp : m_Lights)
		{
			auto light = kvp.second.lock();
			if (!light)
			{
				ME_LOG_WARNING("Light leak");
				continue;
			}

		    auto lightCmp = light->GetComponent<LightComponent>().lock();
			if(!lightCmp || !lightCmp->IsEnabled())
				continue;

			g_PerFrameCBData.m_Lights[lightIndex] = lightCmp->GetData();
			++lightIndex;
			if (lightIndex >= MAX_NUM_LIGHTS)
				break;
		}

		meDevice->ClearRenderTarget(m_OffscreenRenderTargets[0].get(), Color{ 1.0f, 0.0f, 1.0f, 1.0f });

		for (auto& it : m_Cameras)
		{
			auto camera = it.second.lock();
			if (!camera)
			{
				ME_LOG_WARNING("Camera leak");
				continue;
			}

			if(SetupView(*camera))
			{
			    m_RootNode.PreRender();
			    m_RootNode.Render();
			    m_RootNode.PostRender();
				RenderPostEffects();
			}
		}
		const bool success = D3D11Device::GetInstance()->Present(1);
		ME_ASSERT(success);
	}

	bool Renderer::SetupView(Entity& camera)
	{
		const auto cameraComp = camera.GetComponent<CameraComponent>().lock();
		const auto cameraTransform = camera.GetComponent<TransformComponent>().lock();
		if (!cameraComp || !cameraTransform || !cameraComp->IsEnabled())
			return false;

		D3D11Device* meDevice = D3D11Device::GetInstance();

		meDevice->SetTextureAsRenderTarget(m_OffscreenRenderTargets[0].get(), true);
		meDevice->ClearDSV(D3D11_CLEAR_DEPTH, 1.0f, 0);

		D3D11_VIEWPORT viewPort = cameraComp->GetViewPort(); //camera component stores viewport in normalized values
		viewPort.Width *= meDevice->GetSwapchainWidth();
		viewPort.Height *= meDevice->GetSwapchainHeight();
		viewPort.TopLeftX *= meDevice->GetSwapchainWidth();
		viewPort.TopLeftY *= meDevice->GetSwapchainHeight();
	    meDevice->SetViewport(&viewPort);

		g_PerFrameCBData.m_CameraPos = cameraTransform->GetPosition();
		g_PerFrameCBData.m_ViewProjectionMatrix = DirectX::XMMatrixTranspose(cameraComp->GetView() * cameraComp->GetProjection());
		g_PerFrameCBData.m_NearPlane = cameraComp->GetNearPlane();
	    g_PerFrameCBData.m_FarPlane = cameraComp->GetFarPlane();

		m_PerFrameCB.Write(&g_PerFrameCBData, sizeof(g_PerFrameCBData));

		meDevice->SetConstantBufferVS(0, &m_PerFrameCB);
		meDevice->SetConstantBufferPS(0, &m_PerFrameCB);

		return true;
	}

	void Renderer::RenderPostEffects()
    {
		D3D11Device* meDevice = D3D11Device::GetInstance();

		int sourceIdx = 0;
		int targetIdx = 1;
		int count = 1;
		for(auto& kvp : m_PostEffects)
		{
			D3D11Texture2D* target = count == m_PostEffects.size() ? meDevice->GetBackBuffer() : m_OffscreenRenderTargets[targetIdx].get();
			kvp.second->Render(m_OffscreenRenderTargets[sourceIdx].get(), target);
		    meDevice->SetTextureAsShaderResourcePS(0, nullptr);
			std::swap(sourceIdx, targetIdx);
			++count;
		}
    }

	void Renderer::OnWindowResize(int width, int height, bool isMinimized)
	{
		ME_UNUSED(isMinimized);

		D3D11Device* meDevice = D3D11Device::GetInstance();
		meDevice->ResizeSwapchain(width, height);

		m_RootNode.OnWindowResize(width, height);

		for (int i = 0; i < 2; ++i)
		{
			meDevice->ReleaseRenderTarget(RenderTargetSize::Full, m_OffscreenRenderTargets[i]);
			m_OffscreenRenderTargets[i] = meDevice->AcquireRenderTarget(RenderTargetSize::Full);
		}
	}
};