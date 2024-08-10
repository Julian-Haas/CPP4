//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "MicroEngine\component.h"

namespace capp
{
	namespace AppComponentID
	{
		enum Enum : me::ComponentID
		{
			RESERVED_FIRST = me::EngineComponentID::FIRST,
			//Dont add anything in between!
			RESERVED_LAST = me::EngineComponentID::COUNT - 1,
		};

	};
};