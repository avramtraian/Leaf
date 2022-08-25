// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "CoreTypes.h"

namespace Leaf {

	class LEAF_API Memory
	{
	public:
		static bool Initialize();

		static void Shutdown();

	public:
		LF_NODISCARD static void* AllocateRaw(uint64 size);

		LF_NODISCARD static void* Allocate(uint64 size);

		LF_NODISCARD static void* AllocateTagged(uint64 size, const char* file, const char* function, uint32 line);

	public:
		static void FreeRaw(void* block);

		static void Free(void* block);

	public:
		static void Copy(void* destination, const void* source, uint64 size);

		static void Set(void* destination, uint8 value, uint64 size);

		static void Zero(void* destination, uint64 size);
	};

}

#define lnew new (LF_FILE, LF_FUNCTION, LF_LINE)
#define ldelete delete

void* operator new (size_t size);
void* operator new (size_t size, const char* file, const char* function, Leaf::uint32 line);
void* operator new (size_t, void* address) noexcept;

void operator delete(void* block);