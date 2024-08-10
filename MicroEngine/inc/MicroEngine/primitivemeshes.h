//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "me_interface.h"

namespace me
{
	class Mesh;
	class Material;

	ME_API Mesh* CreateCube(float width, float height, float depth, const Material& material);

	ME_API Mesh* CreatePlane(float patchWidth, float patchDepth, int patchesX, int patchesZ, const Material& material);
};