// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	namespace StringCalls {

		LEAF_API uint64 Length(const char* string);

	}

	namespace StringCallsTemplated {

		template<typename CharType>
		inline uint64 Length(const CharType* string);

		template<>
		inline uint64 Length(const char* string)
		{
			return StringCalls::Length(string);
		}

	}

}