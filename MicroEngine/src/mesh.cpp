//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "mesh.h"
#include "d3d11device.h"
#include "vertex.h"

namespace me
{
	Mesh::Mesh(D3D11_PRIMITIVE_TOPOLOGY topology, const VertexDescription& vertexDescription)
		: m_Topology(topology)
		, m_VertexDescription(vertexDescription)
		, m_NumVertices(0)
		, m_NumIndices(0)
	{

	}

	void Mesh::AddIndexedTriangle(uint32_t v1Index, uint32_t v2Index, uint32_t v3Index)
	{
		m_Indices.push_back(v1Index);
		m_Indices.push_back(v2Index);
		m_Indices.push_back(v3Index);
	}

	size_t Mesh::GetNumVertices() const
	{
		return (m_Vertices.size() * sizeof(float)) / (m_VertexDescription.m_VertexSize);
	}

	bool Mesh::CreateBuffers()
	{
		m_NumIndices = 0;
		m_NumVertices = 0;

		if (!m_Indices.empty())
		{
			if (!m_IndexBuffer.CreateStatic(m_Indices.data(), m_Indices.size() * sizeof(uint32_t), D3D11_BIND_INDEX_BUFFER))
				return false;
		}

        const bool success = m_VertexBuffer.CreateStatic(m_Vertices.data(), m_Vertices.size() * sizeof(float), D3D11_BIND_VERTEX_BUFFER);
		if (success)
		{
			m_NumIndices = GetNumIndices();
			m_NumVertices = GetNumVertices();
		}
		return success;
	}

	void Mesh::ReleaseBuffers()
    {
        m_IndexBuffer.Release();
		m_VertexBuffer.Release();
    }


	void Mesh::Set(uint32_t slot)
	{
		D3D11Device* meDevice = D3D11Device::GetInstance();
		if (IsIndexed())
		{
			meDevice->SetIndexBuffer(&m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		}

		meDevice->SetVertexBuffer(slot, &m_VertexBuffer, m_VertexDescription.m_VertexSize, 0);
		meDevice->SetPrimitiveTopology(m_Topology);
	}
};