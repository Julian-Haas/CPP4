//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include <d3d11.h>
#include "util.h"
#include "comptr.h"
#include "me_interface.h"

namespace me
{
	

	class D3D11Buffer
	{
	public:
		ME_API D3D11Buffer();
		ME_API ~D3D11Buffer() = default;

		ME_API bool CreateStatic(const void* data, size_t sizeBytes, uint32_t bindFlags);
		ME_API bool CreateDynamic(size_t sizeBytes, uint32_t bindFlags);

		ME_API void Release();
		bool Write(void* data) { return Write(data, GetSize()); }
		ME_API bool Write(const void* data, size_t size);

		bool IsInit() const { return m_IsInit; }

		size_t GetSize() const { return m_Size; }
		operator ID3D11Buffer*() { return m_Buffer; }
		operator const ID3D11Buffer*() { return m_Buffer; }
	private:
		ME_MOVE_COPY_NOT_ALLOWED(D3D11Buffer);

		friend class D3D11Device;
		ID3D11Buffer* GetBuffer() { return m_Buffer; }

		bool m_IsInit;
		bool m_IsDynamic;
		size_t m_Size;
		ComPtr<ID3D11Buffer> m_Buffer;
	};

};
