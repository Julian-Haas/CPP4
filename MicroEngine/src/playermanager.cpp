#include "stdafx.h"
#include "playermanager.h"
#include "transformcomponent.h"
#include "meshrenderercomponent.h"
#include "material.h"
#include "primitivemeshes.h"
#include "say.h"
#include "mesh.h"

namespace me
{
	PlayerManager::PlayerManager(EntityManager& entityManager)
		: m_entityManagerReference(entityManager)
	{
	}
	void PlayerManager::ProcessIncomingPlayerData(float incomingData[5])
	{
		int playerID = (int)incomingData[1];
		int x = (int)incomingData[2];
		int y = (int)incomingData[3];
		int z = (int)incomingData[4];
		auto it = m_PlayerEntities.find(playerID);
		if (it == m_PlayerEntities.end())
		{
			EntityID entityID = InstantiateNewPlayer();
			m_PlayerEntities[playerID] = entityID;
			return;
		}
		EntityID entityID = it->second;
		m_entityManagerReference.SetPositionByID(entityID, (float)x, (float)y, (float)z);
		return;
	}
	EntityID PlayerManager::InstantiateNewPlayer()
	{
		using namespace me;
		const auto cube = m_entityManagerReference.AddEntity();
		auto transform = cube->GetComponent<TransformComponent>().lock();
		transform->Translate(0.0f, 10.0f, 30.0f);
		auto meshRenderer = m_entityManagerReference.AddComponent<MeshRendererComponent>(cube->GetID());
		Material cubeMat;
		cubeMat.AddShaderProperty(Color::s_White);
		cubeMat.AddShaderProperty(Color::s_Black);
		cubeMat.AddShaderProperty(Color::s_White);
		cubeMat.AddShaderProperty(30.0f);
		cubeMat.SetTexturePS(0, TextureInfo("assets://colormap.bmp"));
		cubeMat.SetVertexShader("assets://Mesh.hlsl");
		cubeMat.SetPixelShader("assets://Mesh.hlsl");
		const auto mesh = std::shared_ptr<Mesh>(CreateCube(10, 10, 10, cubeMat));
		meshRenderer->SetMesh(mesh);
		return cube->GetID();
	}
}