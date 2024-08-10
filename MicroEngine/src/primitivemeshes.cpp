//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "primitivemeshes.h"

#include "mesh.h"
#include "vertex.h"
#include "material.h"

namespace me
{
	void AddVertexLitTextured(Mesh* mesh, float x, float y, float z, float xn, float yn, float zn, float u, float v)
	{
		mesh->AddVertex(VertexLitTextured{{x+0.5f, y + 0.5f, z + 0.5f}, {xn, yn, zn}, {u, v}});
	}

	void AddVertexTextured(Mesh* mesh, float x, float y, float z, float u, float v)
	{
		mesh->AddVertex(VertexTextured{{x, y, z}, {u, v}});
	}

	Mesh* CreateCube(float width, float height, float depth, const Material& material)
	{
        const float hWidth = width / 2;
        const float hHeight = height / 2;
        const float hDepth = depth / 2;

		Mesh* mesh = new Mesh(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, VertexLitTextured::s_Description);

		//Top face
		AddVertexLitTextured(mesh, -hWidth, hHeight, -hDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, -hDepth, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, hDepth, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		AddVertexLitTextured(mesh, -hWidth, hHeight, hDepth, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		//Bottom face
		AddVertexLitTextured(mesh, -hWidth, -hHeight, -hDepth, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
		AddVertexLitTextured(mesh, hWidth, -hHeight, -hDepth, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
		AddVertexLitTextured(mesh, hWidth, -hHeight, hDepth, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
		AddVertexLitTextured(mesh, -hWidth, -hHeight, hDepth, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);

		//Left face
		AddVertexLitTextured(mesh, -hWidth, -hHeight, hDepth, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		AddVertexLitTextured(mesh, -hWidth, -hHeight, -hDepth, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		AddVertexLitTextured(mesh, -hWidth, hHeight, -hDepth, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		AddVertexLitTextured(mesh, -hWidth, hHeight, hDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

		//Right face
		AddVertexLitTextured(mesh, hWidth, -hHeight, hDepth, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, -hHeight, -hDepth, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, -hDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, hDepth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		//Front face
		AddVertexLitTextured(mesh, -hWidth, -hHeight, -hDepth, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, -hHeight, -hDepth, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, -hDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		AddVertexLitTextured(mesh, -hWidth, hHeight, -hDepth, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);

		//Back face
		AddVertexLitTextured(mesh, -hWidth, -hHeight, hDepth, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, -hHeight, hDepth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		AddVertexLitTextured(mesh, hWidth, hHeight, hDepth, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		AddVertexLitTextured(mesh, -hWidth, hHeight, hDepth, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

		//Setup triangle indices

		mesh->AddIndexedTriangle(3, 1, 0);
		mesh->AddIndexedTriangle(2, 1, 3);
		mesh->AddIndexedTriangle(6, 4, 5);
		mesh->AddIndexedTriangle(7, 4, 6);
		mesh->AddIndexedTriangle(11, 9, 8);
		mesh->AddIndexedTriangle(10, 9, 11);
		mesh->AddIndexedTriangle(14, 12, 13);
		mesh->AddIndexedTriangle(15, 12, 14);
		mesh->AddIndexedTriangle(19, 17, 16);
		mesh->AddIndexedTriangle(18, 17, 19);
		mesh->AddIndexedTriangle(22, 20, 21);
		mesh->AddIndexedTriangle(23, 20, 22);

		mesh->AddSubMesh(0, mesh->GetNumIndices(), material);

		mesh->CreateBuffers();

		return mesh;
	}

	Mesh* CreatePlane(float width, float depth, int pX, int pZ, const Material& material)
	{
		Mesh* mesh = new Mesh(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, VertexTextured::s_Description);

		const float patchesX = static_cast<float>(pX);
		const float patchesZ = static_cast<float>(pZ);
		const float hx = patchesX * 0.5f;
		const float hz = patchesZ * 0.5f;

		for (int iZ = 0; iZ < pZ; ++iZ)
		{
			const float z = static_cast<float>(iZ);
			for (int iX = 0; iX < pX + 1; ++iX)
			{
				const float x = static_cast<float>(iX);
				AddVertexTextured(mesh, (x - hx) * width, 0.0f, (z + 0 - hz) * depth, x / (patchesX + 1), (z + 0) / patchesZ);
				AddVertexTextured(mesh, (x - hx) * width, 0.0f, (z + 1 - hz) * depth, x / (patchesX + 1), (z + 1) / patchesZ);
			}

			AddVertexTextured(mesh, (patchesX - hx) * width, 0.0f, (z + 1 - hz) * depth, patchesX / (patchesX + 1), (z + 1) / patchesZ);

			AddVertexTextured(mesh, (0 - hx) * width, 0.0f, (z + 1 - hz) * depth, 0.0f / (patchesX + 1), (z + 1) / patchesZ);
		}

		mesh->AddSubMesh(0, mesh->GetNumVertices(), material);

		mesh->CreateBuffers();

		return mesh;
	}
};