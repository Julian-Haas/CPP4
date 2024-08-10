//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include <memory>
#include "me_interface.h"

namespace me
{
	class Mesh;

	ME_API int GetFormatBits(DXGI_FORMAT format);
	ME_API int GetFormatBytes(DXGI_FORMAT format);

	ME_API ID3DBlob* ReadFileToBlob(const std::string& file);
	ME_API ID3DBlob* CompileShader(const std::string& file, const std::string& entryPoint, const std::string& shaderModel);
	ME_API ID3DBlob* CompileShader(const void* src, std::size_t size, const std::string& name, const std::string& entryPoint, const std::string& shaderModel);

	ME_API std::shared_ptr<Mesh> CreateMeshFromFile(const std::string& file);
};
