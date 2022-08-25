// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/Memory.h"

#define AllocateTaggedI(SIZE) AllocateTagged(SIZE, LF_FILE, LF_FUNCTION, LF_LINE) 

namespace Leaf {

	template<typename AllocatorTypeA, typename AllocatorTypeB>
	bool CompareAllocators(const AllocatorTypeA& allocator_a, const AllocatorTypeB& allocator_b)
	{
		if constexpr (AllocatorTypeA::AllocatorID != AllocatorTypeB::AllocatorID)
			return false;

		return (allocator_a == allocator_b);
	}

	class HeapAllocator
	{
	public:
		static constexpr uint64 AllocatorID = 1;

	public:
		static void* AllocateRaw(uint64 size)
		{
			return Memory::AllocateRaw(size);
		}

		static void* Allocate(uint64 size)
		{
			return Memory::Allocate(size);
		}

		static void* AllocateTagged(uint64 size, const char* file, const char* function, uint32 line)
		{
			return Memory::AllocateTagged(size, file, function, line);
		}

	public:
		static void FreeRaw(void* block, uint64 size)
		{
			Memory::FreeRaw(block);
		}

		static void Free(void* block, uint64 size)
		{
			Memory::Free(block);
		}

	public:
		bool operator==(const HeapAllocator& other) const
		{
			return true;
		}
	};

	class UntrackedAllocator
	{
	public:
		static constexpr uint64 AllocatorID = 2;

	public:
		static void* AllocateRaw(uint64 size)
		{
			return Memory::AllocateRaw(size);
		}

		static void* Allocate(uint64 size)
		{
			return Memory::AllocateRaw(size);
		}

		static void* AllocateTagged(uint64 size, const char* file, const char* function, uint32 line)
		{
			return Memory::AllocateRaw(size);
		}

	public:
		static void FreeRaw(void* block, uint64 size)
		{
			Memory::FreeRaw(block);
		}

		static void Free(void* block, uint64 size)
		{
			Memory::FreeRaw(block);
		}

	public:
		bool operator==(const UntrackedAllocator& other) const
		{
			return true;
		}
	};

}