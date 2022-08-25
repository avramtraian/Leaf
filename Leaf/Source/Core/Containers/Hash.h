// Copyright to Avram Traian. 2022 - 2022.
// File created on June 16 2022.

#pragma once

#include "Core/CoreTypes.h"

#define LF_DECL_HASH_PRIMITIVE(T, Bits)                   \
	template<>                                              \
	struct Hasher<T>                                        \
	{                                                       \
	public:                                                 \
		static HashT Get(T const& value)                    \
		{                                                   \
			return (HashT)(*(const uint##Bits##*)&value); \
		}                                                   \
	}

namespace Leaf {

	using HashT = uint64;

	template<typename T>
	struct Hasher
	{
	public:
		/*
		static HashT Get(const T&)
		{
			return HASH_OF_THE_OBJECT;
		}
		*/
	};

	LF_DECL_HASH_PRIMITIVE(uint8, 8);
	LF_DECL_HASH_PRIMITIVE(uint16, 16);
	LF_DECL_HASH_PRIMITIVE(uint32, 32);
	LF_DECL_HASH_PRIMITIVE(uint64, 64);

	LF_DECL_HASH_PRIMITIVE(int8, 8);
	LF_DECL_HASH_PRIMITIVE(int16, 16);
	LF_DECL_HASH_PRIMITIVE(int32, 32);
	LF_DECL_HASH_PRIMITIVE(int64, 64);

	LF_DECL_HASH_PRIMITIVE(float, 32);
	LF_DECL_HASH_PRIMITIVE(double, 64);

	LF_DECL_HASH_PRIMITIVE(const void*, 64);
	LF_DECL_HASH_PRIMITIVE(void*, 64);

	LF_DECL_HASH_PRIMITIVE(char, 16);

	// TODO (Avr): Implement our own string hashing algorithm.
	LEAF_API HashT Hasher_const_char_p_Get(const char* const& value);

	template<>
	struct Hasher<const char*>
	{
	public:
		static HashT Get(const char* const& value)
		{
			return Hasher_const_char_p_Get(value);
		}
	};

	template<>
	struct Hasher<char*>
	{
	public:
		static HashT Get(char* const& value)
		{
			return Hasher<const char*>::Get((const char*)value);
		}
	};

}