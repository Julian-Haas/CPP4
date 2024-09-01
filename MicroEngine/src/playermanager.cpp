#include "stdafx.h"
#include "playermanager.h"

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
		int playerID = (int)incomingData[1];
		int x = (int)incomingData[2];
		int y = (int)incomingData[3];
		int z = (int)incomingData[4];
		auto it = m_PlayerEntities.find(playerID);
		if (it == m_PlayerEntities.end()) {
			EntityID entityID = 3; // create new thing and store id
			m_PlayerEntities[playerID] = entityID;
			return;
		}
		EntityID entityID = it->second;
		//set position
		return;
	}
}