// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreDefines.h"

#if LF_PLATFORM_WINDOWS
#	include <Windows.h>
#	include <Windowsx.h>
#else
#	error Trying to include Windows-only files!
#endif