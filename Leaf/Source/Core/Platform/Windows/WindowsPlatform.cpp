// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "Core/Platform/Platform.h"
#include "Core/Memory.h"

#include "WindowsHeaders.h"
#include <cstdlib>

namespace Leaf {

	struct WindowsPlatformData
	{

	};
	static WindowsPlatformData* s_PlatformData = nullptr;

	bool Platform::Initialize()
	{
		if (s_PlatformData)
			return false;

		s_PlatformData = (WindowsPlatformData*)malloc(sizeof(WindowsPlatformData));
		new (s_PlatformData) WindowsPlatformData();

		return true;
	}

	void Platform::Shutdown()
	{
		if (!s_PlatformData)
			return;

		(*s_PlatformData).~WindowsPlatformData();
		free(s_PlatformData);
		s_PlatformData = nullptr;
	}

	void* Platform::Allocate(uint64 size)
	{
		return malloc(size);
	}

	void Platform::Free(void* block)
	{
		free(block);
	}

}