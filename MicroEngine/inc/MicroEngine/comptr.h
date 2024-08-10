//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#pragma once

#include "util.h"

namespace me
{
	template<class T>
	class ComPtr
	{
	public:
		ComPtr()
			: m_Pointer(nullptr)
		{};

		ComPtr(const ComPtr<T>& other)
			: m_Pointer(nullptr)
		{
		    Set(other);
		};

		ComPtr(T* ptr)
			: m_Pointer(nullptr)
		{
			Set(ptr);
		}

		~ComPtr()
		{
			Set(nullptr);
		};

		ComPtr<T>& operator=(T* other)
		{
			Set(other);
			return *this;
		}

		ComPtr<T>& operator=(const ComPtr<T>& other)
		{
			if (*this != other)
			    Set(other);
			return *this;
		}

		operator T*() { return m_Pointer; }
		T* operator->() { return m_Pointer; }
		const T* operator->() const { return m_Pointer; }
		operator T*() const { return m_Pointer; }

		inline T** GetPointerAddress() { return &m_Pointer; }

		void Reset() { *this = nullptr; }

	private:
		ME_MOVE_NOT_ALLOWED(ComPtr);

		T* m_Pointer;

		void Set(T* ptr)
		{
			if (ptr != nullptr)
				ptr->AddRef();

			ME_SAFE_RELEASE(m_Pointer);
			m_Pointer = ptr;
		}
	};
};