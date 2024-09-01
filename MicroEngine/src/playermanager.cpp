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

}