// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	namespace StringCalls {

		LEAF_API SizeT Length(const char* string);

		LEAF_API bool Equals(const char* string_a, const char* string_b);

	}

	namespace StringCalls_Wide {

		LEAF_API SizeT Length(const wchar_t* string);

	}

	namespace StringCalls_Templated {

		template<typename CharType>
		inline SizeT Length(const CharType* string);

		template<>
		inline SizeT Length(const char* string)
		{
			return StringCalls::Length(string);
		}

		template<>
		inline SizeT Length(const wchar_t* string)
		{
			return StringCalls_Wide::Length(string);
		}

	}

}