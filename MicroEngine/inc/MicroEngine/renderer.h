//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "util.h"
#include "d3d11buffer.h"
#include "scenenode.h"
#include "mesh.h"
#include "d3d11texture2d.h"
#include "system.h"
#include "me_interface.h"
#include "posteffect.h"

namespace me
{
	class Entity;
	class IEvent;

	class Renderer : public ISystem
	{
	public:
		ME_API Renderer();
		ME_API ~Renderer();

		ME_API bool Init(HWND hWnd, int width, int height);
		ME_API virtual void Update(float deltaTime) override;

		template <class T>
		T* GetPostEffect()
		{
			auto it = m_PostEffects.find(T::Pass);
			if (it != m_PostEffects.end())
				return static_cast<T*>(it->second.get());
			return nullptr;
		}

		ME_API void AddPostEffect(std::unique_ptr<PostEffect> postEffect);

		template <class T>
		void RemovePostEffect() { m_PostEffects.erase(T::Pass); }

		ME_API virtual const std::vector<std::vector<ComponentID>>& GetRequiredComponents() const override;

	protected:
		virtual void OnAddEntity(const std::shared_ptr<Entity>& entity) override;
		virtual void OnRemoveEntity(EntityID entityID) override;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Renderer);
		bool SetupView(Entity& camera);
		void RenderPostEffects();
		void OnWindowResize(int width, int height, bool isMinimized);

		std::map<EntityID, std::weak_ptr<Entity>> m_Cameras;
		std::map<EntityID, std::weak_ptr<Entity>> m_Lights;
		RootNode m_RootNode;
		D3D11Buffer m_PerFrameCB;
		std::unique_ptr<D3D11Texture2D> m_OffscreenRenderTargets[2];
		std::map<PostEffectPass, std::unique_ptr<PostEffect>> m_PostEffects;

		bool m_IsInit;
	};

}
