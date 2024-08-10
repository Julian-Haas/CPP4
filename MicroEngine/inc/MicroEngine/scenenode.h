//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "util.h"
#include "d3d11buffer.h"
#include "d3d11samplerstate.h"
#include "d3d11texture2d.h"
#include "me_interface.h"

namespace me
{
	class Entity;
	class Renderer;

	namespace RenderPass
	{
		enum Enum
		{
			FIRST,
			Terrain = FIRST,
			Opaque,
			Shadow,
			Background,
			Transparent,
			UI,	
			COUNT,
			INVALID = COUNT,
		};
	};

	class SceneNode
	{
	public:
		ME_API SceneNode(const std::shared_ptr<Entity>& entity, RenderPass::Enum renderPass);
		ME_API virtual ~SceneNode() = default;

		ME_API virtual void PreRender();
		ME_API virtual void Render();
		ME_API virtual void PostRender();
		ME_API virtual void OnWindowResize(int width, int height);

		ME_API virtual void AddChild(const std::shared_ptr<SceneNode>& child);
		ME_API virtual bool RemoveChild(EntityID id);
		ME_API void RemoveAllChildren();

		const std::weak_ptr<Entity>& GetEntity() const { return m_Entity; }
		EntityID GetEntityID() const { return m_EntityID; }
		RenderPass::Enum GetRenderPass() const { return m_RenderPass; }

	protected:
		ME_MOVE_COPY_NOT_ALLOWED(SceneNode);

		std::vector<std::shared_ptr<SceneNode>>	m_Children;
		std::weak_ptr<Entity> m_Entity;
		EntityID m_EntityID;
		RenderPass::Enum m_RenderPass;
	};

	class RootNode : public SceneNode
	{
	public:
		ME_API RootNode();
		ME_API virtual ~RootNode() = default;
		ME_API virtual void AddChild(const std::shared_ptr<SceneNode>& child) override;
		ME_API virtual bool RemoveChild(EntityID id) override;
	private:
		ME_MOVE_COPY_NOT_ALLOWED(RootNode);
	};

	class MeshRendererNode : public SceneNode
	{
	public:
		ME_API MeshRendererNode(const std::shared_ptr<Entity>& entity, RenderPass::Enum renderPass);
		ME_API virtual ~MeshRendererNode();

		ME_API virtual void Render() override;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(MeshRendererNode);
		static D3D11SamplerState s_SamplerStateWrap;
		static D3D11SamplerState s_SamplerStateClamp;
		static D3D11Buffer s_PerModelCB;
		static size_t s_NodeRefs;

		std::vector<D3D11Texture2D*> m_TmpTextureBuffer;
		std::vector<D3D11SamplerState*> m_TmpSamplerBuffer;
	};
};