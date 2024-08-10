//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "util.h"
#include "component.h"

namespace me
{
	class SceneNode;

	class RendererComponent : public Component
	{
	public:
		static const ComponentID ID = EngineComponentID::RendererComponent;
		virtual ComponentID GetID() const override final { return ID; } //cannot be further overriden, only one render component allowed per entity

		RendererComponent() = default;
		virtual ~RendererComponent() = default;

		const std::shared_ptr<SceneNode>& GetSceneNode()
		{
			if (!m_SceneNode)
				m_SceneNode = CreateSceneNode();
			return m_SceneNode;
		}
		virtual std::shared_ptr<SceneNode> CreateSceneNode() = 0;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(RendererComponent);
		std::shared_ptr<SceneNode> m_SceneNode;
	};

}