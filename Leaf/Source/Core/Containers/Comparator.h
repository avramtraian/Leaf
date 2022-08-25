// Copyright to Avram Traian. 2022 - 2022.
// File created on June 16 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	template<typename T>
	struct Comparator
	{
		static bool Compare(const T& a, const T& b)
		{
			return (a == b);
		}
	};

	template<>
	struct Comparator<const char*>
	{
		static bool Compare(const char* const& a, const char* const& b)
		{
			const char* string_a = a;
			const char* string_b = b;

			while (*string_a && *string_b)
			{
				if (*string_a != *string_b)
					return false;

				string_a++;
				string_b++;
			}
			return (*string_a == *string_b);
		}
	};

}