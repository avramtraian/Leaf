// Copyright to Avram Traian. 2022 - 2022.
// File created on August 29 2022.

#pragma once

#include "Core/CoreTypes.h"

#include <type_traits>

namespace Leaf {

	class RefCounted
	{
	private:
		LF_INLINE uint64 __GetReferenceCount() const { return __m_ReferenceCount; }

		LF_INLINE void __SetReferenceCount(uint64 reference_count) { __m_ReferenceCount = reference_count; }

		LF_INLINE void __IncrementReferenceCount() { __m_ReferenceCount++; }

		LF_INLINE bool __DecrementReferenceCount() { return (__m_ReferenceCount--) <= 1; }

	private:
		uint64 __m_ReferenceCount = 0;

	private:
		template<typename T>
		friend class Ref;
	};

	template<typename T>
	class Ref
	{
	public:
		template<typename... Args>
		static Ref Create(Args&&... args)
		{
			Ref<T> result;
			result.m_Pointer = lnew T(Leaf::Forward<Args>(args)...);
			result.m_Pointer->__IncrementReferenceCount();
			return result;
		}

	public:
		Ref()
			: m_Pointer(nullptr)
		{}

		Ref(const Ref& other)
			: m_Pointer(nullptr)
		{
			if (other.m_Pointer)
			{
				m_Pointer = other.m_Pointer;
				m_Pointer->__IncrementReferenceCount();
			}
		}

		Ref(Ref&& other) noexcept
		{
			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;
		}

		Ref(NullptrT)
			: m_Pointer(nullptr)
		{}

		~Ref()
		{
			Release();
		}

	public:
		Ref& operator=(const Ref& other)
		{
			if (other.m_Pointer)
				other.m_Pointer->__IncrementReferenceCount();

			Release();
			m_Pointer = other.m_Pointer;

			return *this;
		}

		Ref& operator=(Ref&& other) noexcept
		{
			Release();

			m_Pointer = other.m_Pointer;
			other.m_Pointer = nullptr;

			return *this;
		}

		Ref& operator=(NullptrT)
		{
			Release();
			return *this;
		}

	public:
		LF_INLINE T* operator->() { return Get(); }

		LF_INLINE const T* operator->() const { return Get(); }

		LF_INLINE T& operator*() { return *Get(); }

		LF_INLINE const T& operator*() const { return *Get(); }

	public:
		LF_INLINE operator bool() const { return IsValid(); }

		LF_INLINE bool operator==(const Ref& other) const { return m_Pointer == other.m_Pointer; }

		LF_INLINE bool operator==(NullptrT) const { return m_Pointer == nullptr; }

		LF_INLINE bool operator!=(const Ref& other) const { return m_Pointer != other.m_Pointer; }

		LF_INLINE bool operator!=(NullptrT) const { return m_Pointer != nullptr; }

	public:
		LF_INLINE T* Get() { return m_Pointer; }

		LF_INLINE const T* Get() const { return m_Pointer; }

		LF_INLINE bool IsValid() const { return m_Pointer != nullptr; }

		void Release()
		{
			if (!m_Pointer)
				return;

			if (m_Pointer->__DecrementReferenceCount())
				ldelete m_Pointer;

			m_Pointer = nullptr;
		}

	public:
		template<typename Q>
		Ref<Q> As() const
		{
			Ref<Q> casted;
			
			if (m_Pointer)
			{
				casted.m_Pointer = static_cast<Q>(m_Pointer);
				casted.m_Pointer->__IncrementReferenceCount();
			}

			return casted;
		}

	private:
		static_assert(std::is_base_of_v<RefCounted, T>, "T must be derived from RefCounted!");
		T* m_Pointer;

	private:
		template<typename T>
		friend class Weak;
	};

}