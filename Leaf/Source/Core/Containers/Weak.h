// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "Ref.h"

namespace Leaf {

	template<typename T>
	class Weak
	{
	public:
		Weak()
			: m_Pointer(nullptr)
		{}

		Weak(const Weak& other)
			: m_Pointer(other.m_Pointer)
		{}

		Weak(Weak&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
		}

		Weak(NullptrT)
			: m_Pointer(nullptr)
		{}

		Weak(const Ref<T>& ref)
		{
			m_Pointer = ref.m_Pointer;
		}

		~Weak()
		{}

	public:
		Weak& operator=(const Weak& other)
		{
			m_Pointer = other.m_Pointer;
			return *this;
		}

		Weak& operator=(Weak&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
			return *this;
		}

		Weak& operator=(NullptrT)
		{
			Release();
			return *this;
		}

		Weak& operator=(const Ref<T>& ref)
		{
			m_Pointer = ref.m_Pointer;
			return *this;
		}

	public:
		LF_INLINE T* operator->() { return Get(); }

		LF_INLINE const T* operator->() const { return Get(); }

		LF_INLINE T& operator*() { return *Get(); }

		LF_INLINE const T& operator*() const { return *Get(); }

	public:
		LF_INLINE operator bool() const { return IsValid(); }

		LF_INLINE bool operator==(const Weak& other) const { return m_Pointer == other.m_Pointer; }

		LF_INLINE bool operator==(NullptrT) const { return m_Pointer == nullptr; }

		LF_INLINE bool operator==(const Ref<T>& ref) const { return m_Pointer == ref.m_Pointer; }

		LF_INLINE bool operator!=(const Weak& other) const { return m_Pointer != other.m_Pointer; }

		LF_INLINE bool operator!=(NullptrT) const { return m_Pointer != nullptr; }

		LF_INLINE bool operator!=(const Ref<T>& ref) const { return m_Pointer != ref.m_Pointer; }

	public:
		LF_INLINE T* Get() { return m_Pointer; }

		LF_INLINE const T* Get() const { return m_Pointer; }

		LF_INLINE bool IsValid() const { return m_Pointer != nullptr; }

		void Release()
		{
			m_Pointer = nullptr;
		}

	public:
		Ref<T> ToRef() const
		{
			Ref<T> result;

			if (m_Pointer)
			{
				result.m_Pointer = m_Pointer;
				result.m_Pointer->__IncrementReferenceCount();
			}

			return result;
		}

		template<typename Q>
		Weak<Q> As() const
		{
			Weak<Q> result;
			result.m_Pointer = static_cast<Q*>(m_Pointer);
			return result;
		}

	private:
		static_assert(std::is_base_of_v<RefCounted, T>, "T must be derived from RefCounted!");
		T* m_Pointer;
	};

}