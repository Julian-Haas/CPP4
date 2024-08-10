//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <memory>
#include "renderercomponent.h"
#include "me_interface.h"

namespace me
{
	class Mesh;

	class MeshRendererComponent : public RendererComponent
	{
	public:
		ME_API MeshRendererComponent() = default;
		ME_API virtual ~MeshRendererComponent() = default;

		const std::shared_ptr<Mesh>& GetMesh() const { return m_Mesh; }
		void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }

		ME_API virtual std::shared_ptr<SceneNode> CreateSceneNode() override;

	protected:
		std::shared_ptr<Mesh> m_Mesh;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(MeshRendererComponent);
	};

	class TerrainRendererComponent : public MeshRendererComponent
	{
	public:
		ME_API TerrainRendererComponent() = default;
		ME_API virtual ~TerrainRendererComponent() = default;

		ME_API virtual std::shared_ptr<me::SceneNode> CreateSceneNode() override;

	private:
		ME_MOVE_COPY_NOT_ALLOWED(TerrainRendererComponent);
	};
};