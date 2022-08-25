// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	class Platform
	{
	public:
		static bool Initialize();

		static void Shutdown();

	public:
		static void* Allocate(uint64 size);

		static void Free(void* block);
	};

}