// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Core/CoreTypes.h"

namespace Leaf {

	/**
	* 
	*/
	class LEAF_API StringCalls_UTF8
	{
	public:
		/**
		* 
		*/
		static bool BytesToCodepoint(const char* bytes, int32& out_codepoint, uint8& out_advance);

		/**
		* 
		*/
		static bool CodepointToBytes(int32 codepoint, char* bytes_buffer, uint8& out_advance);

		/**
		* 
		*/
		static SizeT Length(const char* utf8_string);

		/**
		* 
		*/
		static bool LengthAndSize(const char* utf8_string, SizeT& out_length, SizeT& out_size);

		/**
		* 
		*/
		static SizeT GetCharacterWidth(const char* utf8_character);

		/**
		* 
		*/
		static bool Equals(const char* utf8_string_a, const char* utf8_string_b);
	};

	/**
	* 
	*/
	class LEAF_API StringCalls_UTF16
	{
	public:
		/**
		* 
		*/
		static bool BytesToCodepoint(const wchar_t* bytes, int32& out_codepoint, uint8& out_advance);

		/**
		* 
		*/
		static bool CodepointToBytes(int32 codepoint, wchar_t* bytes_buffer, uint8& out_advance);

		/**
		* 
		*/
		static SizeT Length(const wchar_t* utf16_string);

		/**
		* 
		*/
		static bool LengthAndSize(const wchar_t* utf16_string, SizeT& out_length, SizeT& out_size);

		/**
		* 
		*/
		static SizeT GetCharacterWidth(const wchar_t* utf16_character);
	};

	/**
	* 
	*/
	class LEAF_API StringCalls_ASCII
	{
	public:
		/**
		*
		*/
		constexpr static bool BytesToCodepoint(const char* bytes, int32& out_codepoint, uint8& out_advance)
		{
			out_advance = 1;
			out_codepoint = (int32)(bytes[0]);
			return true;
		}

		/**
		*
		*/
		constexpr static bool CodepointToBytes(int32 codepoint, char* bytes_buffer, uint8& out_advance)
		{
			out_advance = 1;
			bytes_buffer[0] = (char)(codepoint);
			return true;
		}

		/**
		*
		*/
		static SizeT Length(const char* utf16_string);

		/**
		*
		*/
		static bool LengthAndSize(const char* ascii_string, SizeT& out_length, SizeT& out_size);

		/**
		*
		*/
		constexpr static SizeT GetCharacterWidth(const char* ascii_character)
		{
			return 1;
		}
	};

	/**
	* 
	*/
	struct UTF8Encoding
	{
		/**  */
		using CharacterType = char;

		/**  */
		using StringCallsClass = StringCalls_UTF8;
	};

	/**
	*
	*/
	struct UTF16Encoding
	{
		/**  */
		using CharacterType = wchar_t;

		/**  */
		using StringCallsClass = StringCalls_UTF16;
	};

	/**
	* 
	*/
	struct ASCIIEncoding
	{
		/**  */
		using CharacterType = char;

		/**  */
		using StringCallsClass = StringCalls_ASCII;
	};

}