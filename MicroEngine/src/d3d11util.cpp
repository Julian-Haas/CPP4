//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include <d3dcompiler.h>
#include "d3d11util.h"
#include "d3d11buffer.h"
#include "vertex.h"
#include "mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader\tiny_obj_loader.h"

namespace me
{
	int GetFormatBits(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return 64;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return 32;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return 16;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
			return 8;

		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			return 128;

		case DXGI_FORMAT_R1_UNORM:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			return 64;

		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return 32;

		case DXGI_FORMAT_UNKNOWN:
		default:
			ME_ASSERT(false);
			return 0;
		}
	}

	int GetFormatBytes(DXGI_FORMAT format)
	{
		return GetFormatBits(format) / 8;
	}

	ID3DBlob* ReadFileToBlob(const std::string& file)
	{
		ID3DBlob* blob;
		if (Failed(D3DReadFileToBlob(UTF8ToUTF16(file).c_str(), &blob)))
		{
			ME_LOG_ERROR("Unable to read file to blob: %s", file.c_str());
			return nullptr;
		}
		return blob;
	}

	ID3DBlob* CompileShader(const std::string& file, const std::string& entryPoint, const std::string& shaderModel)
	{
		ME_ASSERTF(EndsWith(file, ".hlsl"), "Invalid file extension for shader source: %s", file.c_str());
		std::vector<uint8_t> data;
		if (!ReadFileContents(file, data))
			return nullptr;

		return CompileShader(reinterpret_cast<const void*>(data.data()), data.size(), file,  entryPoint, shaderModel);
	}

	ID3DBlob* CompileShader(const void* src, std::size_t size, const std::string& name, const std::string& entryPoint, const std::string& shaderModel)
	{
		UINT flags = 0;
        #ifdef TARGET_DEBUG
		    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif
		ID3DBlob* vsBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

        const HRESULT result = D3DCompile(src, size, name.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderModel.c_str(), flags, 0, &vsBlob, errorBlob.GetPointerAddress());
		if (errorBlob != nullptr)
		{
			ME_LOG_ERROR("Shader compilation error: %s", static_cast<const char*>(errorBlob->GetBufferPointer()));
			return nullptr;
		}

		if (Failed(result))
			return nullptr;

		return vsBlob;
	}

	static void CalcNormal(float N[3], const float v0[3], const float v1[3], const float v2[3]) {
		float v10[3];
		v10[0] = v1[0] - v0[0];
		v10[1] = v1[1] - v0[1];
		v10[2] = v1[2] - v0[2];

		float v20[3];
		v20[0] = v2[0] - v0[0];
		v20[1] = v2[1] - v0[1];
		v20[2] = v2[2] - v0[2];

		N[0] = v20[1] * v10[2] - v20[2] * v10[1];
		N[1] = v20[2] * v10[0] - v20[0] * v10[2];
		N[2] = v20[0] * v10[1] - v20[1] * v10[0];

        const float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
		if (len2 > 0.0f) {
            const float len = sqrtf(len2);

			N[0] /= len;
			N[1] /= len;
		}
	}

	std::shared_ptr<Mesh> CreateMeshFromFile(const std::string& file)
	{

		const std::string path = ResolveURI(file);
		std::shared_ptr<Mesh> mesh;
		if (!EndsWith(path, ".obj"))
		{
			ME_LOG_ERROR("Can only load .obj files");
			return mesh;
		}

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
		{
			ME_LOG_ERROR("Failed to read obj file: %s", err.c_str());
			return mesh;
		}

		if (!err.empty())
		{
			ME_LOG_WARNING("%s", err.c_str());
		}

		materials.emplace_back(); //add a default material

		std::vector<Material> meMaterials;
		for (auto& it : materials)
		{
			Material material;
			material.AddShaderProperty(Color(it.diffuse[0], it.diffuse[1], it.diffuse[2]));
			material.AddShaderProperty(Color(it.emission[0], it.emission[1], it.emission[2]));
			material.AddShaderProperty(Color(it.specular[0], it.specular[1], it.specular[2]));
			material.AddShaderProperty(it.shininess);

			material.SetTextureVS(0, TextureInfo(it.displacement_texname, it.displacement_texopt.clamp));
			material.SetTexturePS(0, TextureInfo(it.diffuse_texname, it.diffuse_texopt.clamp));
			material.SetTexturePS(1, TextureInfo(it.specular_texname, it.specular_texopt.clamp));
			material.SetTexturePS(2, TextureInfo(it.specular_highlight_texname, it.specular_highlight_texopt.clamp));
			material.SetTexturePS(3, TextureInfo(it.normal_texname, it.normal_texopt.clamp));
			material.SetTexturePS(4, TextureInfo(it.alpha_texname, it.alpha_texopt.clamp));
			material.SetTexturePS(5, TextureInfo(it.reflection_texname, it.reflection_texopt.clamp));

			meMaterials.push_back(material);
		}

		mesh = std::make_shared<Mesh>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexLitTextured::s_Description);

		size_t totalIndices = 0;
		for (size_t s = 0; s < shapes.size(); s++)
		{
			for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
			{
                const tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                const tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                const tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

				float tc[3][2];
				if (!attrib.texcoords.empty())
				{
					ME_ASSERT(attrib.texcoords.size() > static_cast<std::size_t>(2 * idx0.texcoord_index + 1));
					ME_ASSERT(attrib.texcoords.size() > static_cast<std::size_t>(2 * idx1.texcoord_index + 1));
					ME_ASSERT(attrib.texcoords.size() > static_cast<std::size_t>(2 * idx2.texcoord_index + 1));
					tc[0][0] = attrib.texcoords[idx0.texcoord_index * 2];
					tc[0][1] = 1.0f - attrib.texcoords[idx0.texcoord_index * 2 + 1];
					tc[1][0] = attrib.texcoords[idx1.texcoord_index * 2];
					tc[1][1] = 1.0f - attrib.texcoords[idx1.texcoord_index * 2 + 1];
					tc[2][0] = attrib.texcoords[idx2.texcoord_index * 2];
					tc[2][1] = 1.0f - attrib.texcoords[idx2.texcoord_index * 2 + 1];
				}
				else
				{
					tc[0][0] = 0.0f;
					tc[0][1] = 0.0f;
					tc[1][0] = 0.0f;
					tc[1][1] = 0.0f;
					tc[2][0] = 0.0f;
					tc[2][1] = 0.0f;
				}

				float v[3][3];
				for (int k = 0; k < 3; k++)
				{
                    const int f0 = idx0.vertex_index;
                    const int f1 = idx1.vertex_index;
                    const int f2 = idx2.vertex_index;
					ME_ASSERT(f0 >= 0);
					ME_ASSERT(f1 >= 0);
					ME_ASSERT(f2 >= 0);

					v[0][k] = attrib.vertices[3 * f0 + k];
					v[1][k] = attrib.vertices[3 * f1 + k];
					v[2][k] = attrib.vertices[3 * f2 + k];
				}

				float n[3][3];
				if (!attrib.normals.empty())
				{
                    const int f0 = idx0.normal_index;
                    const int f1 = idx1.normal_index;
                    const int f2 = idx2.normal_index;
					ME_ASSERT(f0 >= 0);
					ME_ASSERT(f1 >= 0);
					ME_ASSERT(f2 >= 0);
					for (int k = 0; k < 3; k++) {
						n[0][k] = attrib.normals[3 * f0 + k];
						n[1][k] = attrib.normals[3 * f1 + k];
						n[2][k] = attrib.normals[3 * f2 + k];
					}
				}
				else
				{
					CalcNormal(n[0], v[0], v[1], v[2]);
					n[1][0] = n[0][0];
					n[1][1] = n[0][1];
					n[1][2] = n[0][2];
					n[2][0] = n[0][0];
					n[2][1] = n[0][1];
					n[2][2] = n[0][2];
				}

				for (int k = 0; k < 3; k++)
				{
					VertexLitTextured vertex = {};
					memcpy(vertex.m_Position, v[k], 3 * sizeof(float));
					memcpy(vertex.m_Normal, n[k], 3 * sizeof(float));
					memcpy(vertex.m_UV, tc[k], 2 * sizeof(float));
					mesh->AddVertex(vertex);
				}
			}

			Material* material = &meMaterials[meMaterials.size() - 1]; //default material
			if (!shapes[s].mesh.material_ids.empty() && shapes[s].mesh.material_ids[0] >= 0)
			{
				material = &meMaterials[shapes[s].mesh.material_ids[0]];
			}

			mesh->AddSubMesh(totalIndices, shapes[s].mesh.indices.size(), *material);

			totalIndices += shapes[s].mesh.indices.size();
		}

		mesh->CreateBuffers();

		return mesh;
	}
};