#pragma once

#include "me_interface.h"
#include "util.h"
#include "entitymanager.h"

namespace me
{
	class PlayerManager
	{
	public:
		ME_API PlayerManager(EntityManager& entityManager);
		ME_API ~PlayerManager() = default;
		void ProcessIncomingPlayerData(float incomingData[5]);
		EntityID InstantiateNewPlayer();
	private:
		EntityManager& m_entityManagerReference;
		std::map<int, EntityID> m_PlayerEntities;
	};
}