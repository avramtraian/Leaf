// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Assert.h"

namespace Leaf {

	template<typename T>
	class Unique
	{
	public:
		template<typename... Args>
		static Unique Create(Args&&... args)
		{
			Unique<T> result;
			result.m_Pointer = lnew T(Leaf::Forward<Args>(args)...);
			return result;
		}

	public:
		Unique()
			: m_Pointer(nullptr)
		{}

		Unique(const Unique&) = delete;

		Unique(Unique&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
		}

		Unique(NullptrT)
			: m_Pointer(nullptr)
		{}

		~Unique()
		{
			Release();
		}

	public:
		Unique& operator=(const Unique&) = delete;

		Unique& operator=(Unique&& other) noexcept
		{
			Release();

			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;

			return *this;
		}

		Unique& operator=(NullptrT)
		{
			Release();
			return *this;
		}

	public:
		LF_INLINE operator bool() const { return IsValid(); }

		LF_INLINE bool operator==(const Unique& other) const { return m_Pointer == other.m_Pointer; }

		LF_INLINE bool operator==(NullptrT) const { return m_Pointer == nullptr; }

		LF_INLINE bool operator!=(const Unique& other) const { return m_Pointer != other.m_Pointer; }

		LF_INLINE bool operator!=(NullptrT) const { return m_Pointer != nullptr; }

	public:
		LF_INLINE T* operator->() { return Get(); }

		LF_INLINE const T* operator->() const { return Get(); }

		LF_INLINE T& operator*() { return *Get(); }

		LF_INLINE const T& operator*() const { return *Get(); }

	public:
		LF_INLINE T* Get() { LF_DEBUG_ASSERT(IsValid()); return m_Pointer; }

		LF_INLINE const T* Get() const { LF_DEBUG_ASSERT(IsValid()); return m_Pointer; }

		LF_INLINE bool IsValid() const { return m_Pointer != nullptr; }

		void Release()
		{
			if (!m_Pointer)
				return;

			ldelete m_Pointer;
			m_Pointer = nullptr;
		}

	public:
		template<typename Q>
		Unique<Q> As()
		{
			Unique<Q> result;

			result.m_Pointer = static_cast<Q*>(m_Pointer);
			m_Pointer = nullptr;

			return result;
		}

	private:
		T* m_Pointer;
	};

}