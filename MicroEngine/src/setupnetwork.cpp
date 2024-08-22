#include "stdafx.h"
#include "setupnetwork.h"

void me::SetupNetwork::Testfunktion()
{
	std::cout << "Testfunktion aufgerufen." << std::endl;
}

me::SetupNetwork::SetupNetwork() : server()
, _playerID(0)
, _position_x(0)
, _position_y(0)
, _position_z(30)
{
}

me::SetupNetwork::~SetupNetwork() = default;
