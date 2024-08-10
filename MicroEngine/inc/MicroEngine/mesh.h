//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include <vector>
#include "d3d11buffer.h"
#include "util.h"
#include "material.h"
#include "me_interface.h"

namespace me
{
	struct VertexDescription;

	class Mesh
	{
	public:
		ME_API Mesh(D3D11_PRIMITIVE_TOPOLOGY topology, const VertexDescription& vertexDescription);
		ME_API ~Mesh() = default;

		template <class T>
		void AddVertex(const T& vertex)
		{
			ME_ASSERT(&T::s_Description == &m_VertexDescription);
			const float* buf = reinterpret_cast<const float*>(&vertex);
			m_Vertices.insert(m_Vertices.end(), buf, buf + sizeof(T) / sizeof(float));
		}

		template <class T>
		void AddVertices(const T* vertices, size_t count)
		{
			ME_ASSERT(&T::s_Description == &m_VertexDescription);
			const float* buf = reinterpret_cast<const float*>(vertices);
			m_Vertices.insert(m_Vertices.end(), buf, buf + ((sizeof(T) * count) / sizeof(float)));
		}

		void ME_API AddIndexedTriangle(uint32_t v1Index, uint32_t v2Index, uint32_t v3Index);
		bool ME_API CreateBuffers();
		void ME_API ReleaseBuffers();

		void ME_API Set(uint32_t slot);

		struct SubMesh
		{
			size_t m_Start;
			size_t m_Count;
			Material m_Material;
		};
		std::vector<SubMesh>& GetSubMeshes() { return m_Submeshes; }
		void AddSubMesh(size_t start, size_t count, const Material& material) { m_Submeshes.push_back(SubMesh{ start, count, material }); }

		bool IsIndexed() const { return m_IndexBuffer.IsInit(); }

		size_t ME_API GetNumVertices() const;
		size_t GetNumIndices() const { return m_Indices.size(); }

		size_t GetNumVerticesBuffered() const { return m_NumVertices; }
		size_t GetNumIndicesBuffered() const { return m_NumIndices; }
		const VertexDescription& GetVertexDescription() const { return m_VertexDescription; }

	private:
		ME_MOVE_COPY_NOT_ALLOWED(Mesh);

		std::vector<SubMesh> m_Submeshes;
		std::vector<float> m_Vertices;
		std::vector<uint32_t> m_Indices;

		D3D11Buffer m_VertexBuffer;
		D3D11Buffer m_IndexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY m_Topology;
		const VertexDescription& m_VertexDescription;
		size_t m_NumVertices;
		size_t m_NumIndices;
	};

};