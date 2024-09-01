#include "stdafx.h"
#include "playermanager.h"
#include "transformcomponent.h"
#include "meshrenderercomponent.h"
#include "material.h"
#include "primitivemeshes.h"
#include "say.h"

namespace me {

	PlayerManager::PlayerManager(EntityManager& entityManager)
		: m_entityManagerReference(entityManager)
	{

	}

	PlayerManager::~PlayerManager()
	{
	}

	ME_API void PlayerManager::ProcessIncomingPlayerData(float incomingData[5])
	{
		system("cls");
		for (const auto& pair : m_PlayerEntities) {
			std::cout << "PlayerID: " << pair.first << ", EntityID: " << pair.second << std::endl;
		}
		int playerID = (int)incomingData[1];
		int x = (int)incomingData[2];
		int y = (int)incomingData[3];
		int z = (int)incomingData[4];
		auto it = m_PlayerEntities.find(playerID);
		if (it == m_PlayerEntities.end()) {
			EntityID entityID = InstantiateNewPlayer();
			m_PlayerEntities[playerID] = entityID;
			return;
		}
		EntityID entityID = it->second;
		//set position
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

#pragma warning(push)
#pragma warning(disable: 4150)
		const auto mesh = std::shared_ptr<Mesh>(CreateCube(10, 10, 10, cubeMat));
#pragma warning(pop)
		//const auto mesh = std::shared_ptr<Mesh>(CreateCube(10, 10, 10, cubeMat));
		meshRenderer->SetMesh(mesh);

		return cube->GetID();
	}
}