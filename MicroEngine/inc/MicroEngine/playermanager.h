#pragma once
#include "me_interface.h"
#include "util.h"
#include "entitymanager.h"

namespace me {
	class PlayerManager
	{
	public:
		ME_API PlayerManager(EntityManager& entityManager);
		ME_API ~PlayerManager();
		ME_API void ProcessIncomingPlayerData(float incomingData[5]);
		ME_API EntityID InstantiateNewPlayer();
	private:
		EntityManager& m_entityManagerReference;
		std::map<int, EntityID> m_PlayerEntities;
	};
}
