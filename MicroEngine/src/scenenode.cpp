//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "scenenode.h"
#include "meshrenderercomponent.h"
#include "cameracomponent.h"
#include "color.h"
#include "d3d11vertexshader.h"
#include "d3d11pixelshader.h"
#include "mesh.h"
#include "d3d11device.h"
#include "transformcomponent.h"
#include "entity.h"
#include "renderer.h"

namespace me
{
	SceneNode::SceneNode(const std::shared_ptr<Entity>& entity, RenderPass::Enum renderPass)
		: m_Entity(entity)
		, m_EntityID(entity ? entity->GetID() : INVALID_ENTITY_ID)
		, m_RenderPass(renderPass)
	{
	}

	void SceneNode::PreRender()
	{
		for (auto& child : m_Children)
		{
			child->PreRender();
		}
	}

	void SceneNode::Render()
	{
		for (auto& child : m_Children)
		{
			child->Render();
		}
	}

	void SceneNode::PostRender()
	{
		for (auto& child : m_Children)
		{
			child->PostRender();
		}
	}

	void SceneNode::OnWindowResize(int width, int height)
	{
		for (auto& child : m_Children)
		{
			child->OnWindowResize(width, height);
		}
	}

	void SceneNode::AddChild(const std::shared_ptr<SceneNode>& child)
	{
		m_Children.push_back(child);
	}

	bool SceneNode::RemoveChild(EntityID id)
	{
        const auto it = std::find_if(m_Children.begin(), m_Children.end(), [id](const std::shared_ptr<SceneNode>& node) { return node->GetEntityID() == id; });
		if (it != m_Children.end())
		{
			m_Children.erase(it);
			return true;
		}
		return false;
	}

	void SceneNode::RemoveAllChildren()
	{
		m_Children.clear();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	RootNode::RootNode()
		: SceneNode(std::shared_ptr<Entity>(), RenderPass::INVALID)
	{
		for (int i = RenderPass::FIRST; i < RenderPass::COUNT; ++i)
		{
			m_Children.push_back(std::make_shared<SceneNode>(std::shared_ptr<Entity>(), static_cast<RenderPass::Enum>(i)));
		}
	}

	void RootNode::AddChild(const std::shared_ptr<SceneNode>& child)
	{
		m_Children[child->GetRenderPass()]->AddChild(child);
	}

	bool RootNode::RemoveChild(EntityID entityID)
	{
		for (int i = RenderPass::FIRST; i < RenderPass::COUNT; ++i)
		{
			if (m_Children[i]->RemoveChild(entityID))
				return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct PerModelCBData
	{
		DirectX::XMMATRIX m_WorldMatrix;
	} g_PerModelCBData = {};

	D3D11Buffer MeshRendererNode::s_PerModelCB;
	size_t MeshRendererNode::s_NodeRefs = 0;
	D3D11SamplerState MeshRendererNode::s_SamplerStateWrap;
	D3D11SamplerState MeshRendererNode::s_SamplerStateClamp;

	MeshRendererNode::MeshRendererNode(const std::shared_ptr<Entity>& entity, RenderPass::Enum renderPass)
		: SceneNode(entity, renderPass)
	{
		if (s_NodeRefs == 0)
		{
			s_PerModelCB.CreateDynamic(sizeof(PerModelCBData), D3D11_BIND_CONSTANT_BUFFER);
			s_SamplerStateWrap.Init(D3D11SamplerState::ANISOTROPIC_FILTER, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 4, 0, D3D11_FLOAT32_MAX, 0);
			s_SamplerStateClamp.Init(D3D11SamplerState::ANISOTROPIC_FILTER, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS, 4, 0, D3D11_FLOAT32_MAX, 0);
		}
		++s_NodeRefs;
	}

	MeshRendererNode::~MeshRendererNode()
	{
		--s_NodeRefs;
		if (s_NodeRefs == 0)
		{
			s_PerModelCB.Release();
			s_SamplerStateWrap.Release();
			s_SamplerStateClamp.Release();
		}
	}

	void MeshRendererNode::Render()
	{
        const auto entity = m_Entity.lock();
		if (!entity)
			return;

        const auto transform = entity->GetComponent<TransformComponent>().lock();
        const auto meshRenderer = entity->GetComponent<MeshRendererComponent>().lock();
		if (!transform || !meshRenderer || !meshRenderer->IsEnabled())
			return;

		auto& mesh = meshRenderer->GetMesh();
		if (!mesh)
		{
			ME_LOG_WARNING("MeshRendererComponent has no mesh attached");
			return;
		}

		D3D11Device* meDevice = D3D11Device::GetInstance();

		//Setup vertex and index buffers and topology
		mesh->Set(0);

		//Setup per model cb
		g_PerModelCBData.m_WorldMatrix = DirectX::XMMatrixTranspose(transform->GetWorldMatrix());
		s_PerModelCB.Write(&g_PerModelCBData, sizeof(PerModelCBData));
		meDevice->SetConstantBufferVS(1, &s_PerModelCB);
		meDevice->SetConstantBufferPS(1, &s_PerModelCB);

		for (auto& subMesh : mesh->GetSubMeshes())
		{
			//Setup shaders
			auto* vertexShader = meDevice->FindVertexShader(subMesh.m_Material.GetVertexShader());
		    auto* pixelShader = meDevice->FindPixelShader(subMesh.m_Material.GetPixelShader());
		    meDevice->SetShader(vertexShader);
		    meDevice->SetShader(pixelShader);

			//Setup vertex shader textures and samplers
			m_TmpTextureBuffer.resize(vertexShader->GetNumTextures());
			m_TmpSamplerBuffer.resize(vertexShader->GetNumTextures());
			for(int i = 0; i < subMesh.m_Material.GetTexturesVS().size() && i < m_TmpTextureBuffer.size(); ++i)
			{
				const TextureInfo& info = subMesh.m_Material.GetTexturesVS()[i];
				if (info.GetName().empty())
				    continue;

				m_TmpTextureBuffer[i] = meDevice->FindTexture(info.GetName());
				if (info.IsClamp())
					m_TmpSamplerBuffer[i] = &s_SamplerStateClamp;
				else
					m_TmpSamplerBuffer[i] = &s_SamplerStateWrap;
			}

			meDevice->SetTexturesAsShaderResourcesVS(0, static_cast<uint32_t>(m_TmpTextureBuffer.size()), m_TmpTextureBuffer.data());
			meDevice->SetSamplersVS(0, static_cast<uint32_t>(m_TmpSamplerBuffer.size()), m_TmpSamplerBuffer.data());
			m_TmpTextureBuffer.clear();
		    m_TmpSamplerBuffer.clear();

			//Setup pixel shader textures and samplers
			m_TmpTextureBuffer.resize(pixelShader->GetNumTextures());
			m_TmpSamplerBuffer.resize(pixelShader->GetNumTextures());
			for(int i = 0; i < subMesh.m_Material.GetTexturesPS().size() && i < m_TmpTextureBuffer.size(); ++i)
			{
				const TextureInfo& info = subMesh.m_Material.GetTexturesPS()[i];
				if (info.GetName().empty())
				    continue;

				m_TmpTextureBuffer[i] = meDevice->FindTexture(info.GetName());
				if (info.IsClamp())
					m_TmpSamplerBuffer[i] = &s_SamplerStateClamp;
				else
					m_TmpSamplerBuffer[i] = &s_SamplerStateWrap;
			}

			meDevice->SetTexturesAsShaderResourcesPS(0, static_cast<uint32_t>(m_TmpTextureBuffer.size()), m_TmpTextureBuffer.data());
			meDevice->SetSamplersPS(0, static_cast<uint32_t>(m_TmpSamplerBuffer.size()), m_TmpSamplerBuffer.data());
			m_TmpTextureBuffer.clear();
		    m_TmpSamplerBuffer.clear();

			//Setup shader properties
			if (!subMesh.m_Material.GetShaderProperties().empty())
			{
				if (vertexShader->GetPropertiesBuffer()->IsInit())
			        vertexShader->GetPropertiesBuffer()->Write(reinterpret_cast<const void*>(subMesh.m_Material.GetShaderProperties().data()), subMesh.m_Material.GetShaderProperties().size() * sizeof(float));
				if (pixelShader->GetPropertiesBuffer()->IsInit())
			        pixelShader->GetPropertiesBuffer()->Write(reinterpret_cast<const void*>(subMesh.m_Material.GetShaderProperties().data()), subMesh.m_Material.GetShaderProperties().size() * sizeof(float));
			}

			//Draw
			if (mesh->IsIndexed())
			{
				meDevice->DrawIndexed(static_cast<UINT>(subMesh.m_Count), static_cast<UINT>(subMesh.m_Start), 0);
			}
			else
			{
				meDevice->Draw(static_cast<UINT>(subMesh.m_Count), static_cast<UINT>(subMesh.m_Start));
			}
		}

		SceneNode::Render();
	}

};