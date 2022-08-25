// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "Memory.h"
#include "MemoryOperators.h"

#include "Platform/Platform.h"

#include <cstring>

namespace Leaf {

	struct MemoryData
	{

	};
	static MemoryData* s_MemoryData = nullptr;

	bool Memory::Initialize()
	{
		if (s_MemoryData)
			return false;

		s_MemoryData = (MemoryData*)Platform::Allocate(sizeof(MemoryData));
		new (s_MemoryData) MemoryData();

		return true;
	}

	void Memory::Shutdown()
	{
		if (!s_MemoryData)
			return;

		(*s_MemoryData).~MemoryData();
		Platform::Free(s_MemoryData);
		s_MemoryData = nullptr;
	}

	void* Memory::AllocateRaw(uint64 size)
	{
		if (size == 0)
			return nullptr;

		return Platform::Allocate(size);
	}

	void* Memory::Allocate(uint64 size)
	{
		if (size == 0)
			return nullptr;

		return Memory::AllocateRaw(size);
	}

	void* Memory::AllocateTagged(uint64 size, const char* file, const char* function, uint32 line)
	{
		if (size == 0)
			return nullptr;

		return Memory::AllocateRaw(size);
	}

	void Memory::FreeRaw(void* block)
	{
		Platform::Free(block);
	}

	void Memory::Free(void* block)
	{
		if (block == nullptr)
			return;

		Memory::FreeRaw(block);
	}

	void Memory::Copy(void* destination, const void* source, uint64 size)
	{
		memcpy(destination, source, size);
	}

	void Memory::Set(void* destination, uint8 value, uint64 size)
	{
		memset(destination, value, size);
	}

	void Memory::Zero(void* destination, uint64 size)
	{
		memset(destination, 0, size);
	}

}