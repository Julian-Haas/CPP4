//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "meshrenderercomponent.h"
#include "scenenode.h"

namespace me
{
	std::shared_ptr<SceneNode> MeshRendererComponent::CreateSceneNode()
	{
		return std::make_shared<MeshRendererNode>(GetOwner(), RenderPass::Opaque);
	}

	std::shared_ptr<me::SceneNode> TerrainRendererComponent::CreateSceneNode()
	{
		return std::make_shared<MeshRendererNode>(GetOwner(), RenderPass::Terrain);
	}
};