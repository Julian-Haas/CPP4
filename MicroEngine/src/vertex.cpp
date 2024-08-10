//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "vertex.h"
#include <d3d11.h>

namespace me
{
	D3D11_INPUT_ELEMENT_DESC g_VertexLitTexturedInputElementDesc[] =
	{
		{
			"POSITION", 						// semantic name
			0, 									// semantic index
			DXGI_FORMAT_R32G32B32_FLOAT, 		// format
			0, 									// input slot
			D3D11_APPEND_ALIGNED_ELEMENT, 		// aligned offset
			D3D11_INPUT_PER_VERTEX_DATA, 	    // class
			0									// instance step rate
		},
		{
			"NORMAL",						    // semantic name
			0, 									// semantic index
			DXGI_FORMAT_R32G32B32_FLOAT,		// format
			0, 									// input slot
			D3D11_APPEND_ALIGNED_ELEMENT,       // aligned offset
			D3D11_INPUT_PER_VERTEX_DATA, 	    // class
			0									// instance step rate
		},
		{
			"TEXCOORD",						    // semantic name
			0, 									// semantic index
			DXGI_FORMAT_R32G32_FLOAT, 			// format
			0, 									// input slot
			D3D11_APPEND_ALIGNED_ELEMENT,       // aligned offset
			D3D11_INPUT_PER_VERTEX_DATA, 	    // class
			0									// instance step rate
		},
	};

	D3D11_INPUT_ELEMENT_DESC g_VertexTexturedInputElementDesc[] =
	{
		{
			"POSITION", 						// semantic name
			0, 									// semantic index
			DXGI_FORMAT_R32G32B32_FLOAT, 		// format
			0, 									// input slot
			D3D11_APPEND_ALIGNED_ELEMENT, 		// aligned offset
			D3D11_INPUT_PER_VERTEX_DATA, 	    // class
			0									// instance step rate
		},
		{
			"TEXCOORD",						    // semantic name
			0, 									// semantic index
			DXGI_FORMAT_R32G32_FLOAT, 			// format
			0, 									// input slot
			D3D11_APPEND_ALIGNED_ELEMENT,       // aligned offset
			D3D11_INPUT_PER_VERTEX_DATA, 	    // class
			0									// instance step rate
		},
	};

	VertexDescription VertexLitTextured::s_Description =
	{
		sizeof(VertexLitTextured),
		ME_ARRAY_COUNT(g_VertexLitTexturedInputElementDesc),
		g_VertexLitTexturedInputElementDesc
	};

	VertexDescription VertexTextured::s_Description =
	{
		sizeof(VertexTextured),
		ME_ARRAY_COUNT(g_VertexTexturedInputElementDesc),
		g_VertexTexturedInputElementDesc
	};

};