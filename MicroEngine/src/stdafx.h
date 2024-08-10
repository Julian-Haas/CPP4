//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <map>
#include "windowsinc.h"

#pragma warning (disable: 4324) // 'struct_name' : structure was padded due to __declspec(align())
#pragma warning (disable: 26812) // The enum type 'enum_name' is unscoped. Prefer 'enum class' over 'enum'

#ifdef TARGET_DEBUG
#pragma warning (disable: 4100)
#pragma warning (disable: 4189)
#endif