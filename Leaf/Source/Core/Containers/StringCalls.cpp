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

	bool StringCalls::Equals(const char* string_a, const char* string_b)
	{
		const char* a = string_a;
		const char* b = string_b;

		while ((*a) && (*b))
		{
			if (*a != *b)
				return false;

			a++;
			b++;
		}

		return (*a == *b);
	}

	SizeT StringCalls_Wide::Length(const wchar_t* string)
	{
		const wchar_t* new_string = string;
		while (*(new_string++));
		return new_string - string - 1;
	}

}