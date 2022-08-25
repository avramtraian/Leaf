// Copyright to Avram Traian. 2022 - 2022.
// File created on August 24 2022.

#include "Hash.h"

#include <xhash>

namespace Leaf {

	HashT Hasher_const_char_p_Get(const char* const& value)
	{
		return (HashT)(std::hash<const char*>()(value));
	}

}