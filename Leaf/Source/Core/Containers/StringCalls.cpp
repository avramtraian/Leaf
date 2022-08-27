// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "StringCalls.h"

namespace Leaf {

	uint64 StringCalls::Length(const char* string)
	{
		const char* new_string = string;
		while (*(new_string++));
		return new_string - string - 1;
	}

	SizeT StringCalls_Wide::Length(const wchar_t* string)
	{
		const wchar_t* new_string = string;
		while (*(new_string++));
		return new_string - string - 1;
	}

}