// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#include "StringCalls.h"

namespace Leaf {

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////// UTF-8 ///////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool StringCalls_UTF8::BytesToCodepoint(const char* bytes, int32& out_codepoint, uint8& out_advance)
	{
		int32 codepoint = (int32)(bytes[0]);

		// Single-byte character. Same as ASCII.
		if (0x00 <= codepoint && codepoint <= 0x7F)
		{
			out_advance = 1;
			out_codepoint = codepoint;
		}

		// Double-byte character.
		else if ((codepoint & 0xE0) == 0xC0)
		{
			out_advance = 2;
			out_codepoint =
				((bytes[0] & 0b00011111) << 6) |
				 (bytes[1] & 0b00111111);
		}

		// Triple-byte character.
		else if ((codepoint & 0xF0) == 0xE0)
		{
			out_advance = 3;
			out_codepoint =
				((bytes[0] & 0b00001111) << 12) |
				((bytes[1] & 0b00111111) << 6)  |
				 (bytes[2] & 0b00111111);
		}

		// 4-byte character.
		else if ((codepoint & 0xF8) == 0xF0)
		{
			out_advance = 4;
			out_codepoint =
				((bytes[0] & 0b00000111) << 18) |
				((bytes[1] & 0b00111111) << 12) |
				((bytes[2] & 0b00111111) << 6)  |
				 (bytes[3] & 0b00111111);
		}

		else
		{
			out_advance = 0;
			out_codepoint = 0;

			// Invalid UTF-8.
			LF_ASSERT_RETURN(false, false);
		}

		return true;
	}

	bool StringCalls_UTF8::CodepointToBytes(int32 codepoint, char* bytes_buffer, uint8& out_advance)
	{
		// Single-byte character.
		if (0x00 <= codepoint && codepoint <= 0x7F)
		{
			out_advance = 1;
			bytes_buffer[0] = (char)(codepoint);
		}

		// Double-byte character
		else if (codepoint <= 0x07FF) // Codepoint can be seen as an 11-bit integer.
		{
			out_advance = 2;

			// Extract the first 5 bits.
			bytes_buffer[0] = ((codepoint >> 6)       | 0b11000000);
			
			// Extract the remaining 6-bits.
			bytes_buffer[1] = ((codepoint & 0b111111) | 0b10000000);
		}

		// Triple-byte character
		else if (codepoint <= 0xFFFF) // Codepoint can be seen as a 16-bit integer.
		{
			out_advance = 3;

			// Extract the first 4 bits.
			bytes_buffer[0] =  (codepoint >> 12)            | 0b11100000;

			// Extract the next 6 bits.
			bytes_buffer[1] = ((codepoint >> 6) & 0b111111) | 0b10000000;

			// Extract the remaining 6 bits.
			bytes_buffer[2] =  (codepoint & 0b111111)       | 0b10000000;
		}

		// 4-byte character
		else if (codepoint <= 0x10FFFF) // Codepoint can be seen as a 21-bit integer.
		{
			out_advance = 4;

			// Extract the first 3 bits.
			bytes_buffer[0] =  (codepoint >> 18)             | 0b11110000;

			// Extract the next 6 bits.
			bytes_buffer[1] = ((codepoint >> 12) & 0b111111) | 0b10000000;

			// Extract the next 6 bits.
			bytes_buffer[2] = ((codepoint >> 6) & 0b111111)  | 0b10000000;

			// Extract the remaining 6 bits.
			bytes_buffer[3] = (codepoint & 0b111111)         | 0b10000000;
		}
		
		else
		{
			// Invalid UTF-8.
			LF_ASSERT_RETURN(false, false);
		}

		return true;
	}

	SizeT StringCalls_UTF8::Length(const char* utf8_string)
	{
		SizeT length = 0;

		for (SizeT index = 0; index < UINT64_MAX; index++, length++)
		{
			int32 c = (int32)utf8_string[index];
			if (c == 0)
				break;

			// Single-byte character.
			if (0x00 <= c && c <= 0x7F)  index += 0;

			// Double-byte character.
			else if ((c & 0xE0) == 0xC0) index += 1;

			// Triple-byte character.
			else if ((c & 0xF0) == 0xE0) index += 2;

			// 4-byte character.
			else if ((c & 0xF8) == 0xF0) index += 3;

			else
			{
				// Invalid UTF-8.
				LF_ASSERT_RETURN(false, LF_INVALID_SIZE);
			}
		}

		return length;
	}

	bool StringCalls_UTF8::LengthAndSize(const char* utf8_string, SizeT& out_length, SizeT& out_size)
	{
		SizeT length = 0;
		SizeT index;

		for (index = 0; index < UINT64_MAX; index++, length++)
		{
			int32 c = (int32)utf8_string[index];
			if (c == 0)
				break;

			// Single-byte character.
			if (0x00 <= c && c <= 0x7F)  index += 0;

			// Double-byte character.
			else if ((c & 0xE0) == 0xC0) index += 1;

			// Triple-byte character.
			else if ((c & 0xF0) == 0xE0) index += 2;

			// 4-byte character.
			else if ((c & 0xF8) == 0xF0) index += 3;

			else
			{
				out_length = LF_INVALID_SIZE;
				out_size = LF_INVALID_SIZE;

				// Invalid UTF-8.
				LF_ASSERT_RETURN(false, false);
			}
		}

		out_length = length;
		out_size = index;
		return true;
	}

	SizeT StringCalls_UTF8::GetCharacterWidth(const char* utf8_character)
	{
		int32 codepoint = (int32)(utf8_character[0]);

		// Single-byte character.
		if (0x00 <= codepoint && codepoint <= 0x7F)
			return 1;

		// Double-byte character
		else if (codepoint <= 0x07FF) // Codepoint can be seen as an 11-bit integer.
			return 2;

		// Triple-byte character
		else if (codepoint <= 0xFFFF) // Codepoint can be seen as a 16-bit integer.
			return 3;

		// 4-byte character
		else if (codepoint <= 0x10FFFF) // Codepoint can be seen as a 21-bit integer.
			return 4;

		else
		{
			// Invalid UTF-8.
			LF_ASSERT_NO_ENTRY_RETURN(0);
		}
	}

	bool StringCalls_UTF8::Equals(const char* utf8_string_a, const char* utf8_string_b)
	{
		while (true)
		{
			if (*utf8_string_a == 0)
				break;

			if (*utf8_string_b == 0)
				break;

			if (*utf8_string_a != *utf8_string_b)
				return false;

			utf8_string_a++;
			utf8_string_b++;
		}

		return (*utf8_string_a == *utf8_string_b);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////// UTF-16 //////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool StringCalls_UTF16::BytesToCodepoint(const wchar_t* bytes, int32& out_codepoint, uint8& out_advance)
	{
		int32 codepoint = (int32)(bytes[0]);

		// Single 16-bit character. Same as UCS-2.
		if ((0x0000 <= codepoint && codepoint <= 0xD7FF) || (0xE000 <= codepoint && codepoint <= 0xFFFF))
		{
			out_advance = 1;
			out_codepoint = codepoint;
		}

		// Double 16-bit character.
		else
		{
			int32 hs = bytes[0];
			int32 ls = bytes[1];

			// Invalid UTF-16.
			LF_ASSERT_RETURN(0xDC00 <= ls && ls <= 0xDFFF, false);

			out_advance = 2;
			out_codepoint = ((hs - 0xD800) << 10) + (ls - 0xDC00) + 0x10000;
		}

		return true;
	}

	bool StringCalls_UTF16::CodepointToBytes(int32 codepoint, wchar_t* bytes_buffer, uint8& out_advance)
	{
		// Single 16-bit character. Same as UCS2.
		if ((0x0000 <= codepoint && codepoint <= 0xD7FF) || (0xE000 <= codepoint && codepoint <= 0xFFFF))
		{
			out_advance = 1;
			bytes_buffer[0] = (wchar_t)((int16)codepoint);
		}
		
		// Double 16-bit character.
		else if (0x010000 <= codepoint && codepoint <= 0x10FFFF)
		{
			out_advance = 2;
			int32 cp = codepoint - 0x10000;

			bytes_buffer[0] = (cp >> 10) + 0xD800;
			bytes_buffer[1] = (cp & 0b1111111111) + 0xDC00;
		}

		else
		{
			// Invalid UTF-16.
			LF_ASSERT_RETURN(false, false);
		}

		return true;
	}

	SizeT StringCalls_UTF16::Length(const wchar_t* utf16_string)
	{
		SizeT length = 0;

		for (SizeT index = 0; index < UINT64_MAX; index++, length++)
		{
			int32 c = (int32)(utf16_string[index]);
			if (c == 0)
				break;

			// Single 16-bit character.
			if ((0x0000 <= c && c <= 0xD7FF) || (0xE000 <= c && c <= 0xFFFF))
				index += 0;

			// Double 16-bit character.
			else if (0x010000 <= c && c <= 0x10FFFF)
				index += 1;

			else
			{
				// Invalid UTF-16.
				LF_ASSERT_RETURN(false, LF_INVALID_SIZE);
			}
		}

		return length;
	}

	bool StringCalls_UTF16::LengthAndSize(const wchar_t* utf16_string, SizeT& out_length, SizeT& out_size)
	{
		SizeT length = 0;
		SizeT index;

		for (index = 0; index < UINT64_MAX; index++, length++)
		{
			int32 c = (int32)(utf16_string[index]);
			if (c == 0)
				break;

			// Single 16-bit character.
			if ((0x0000 <= c && c <= 0xD7FF) || (0xE000 <= c && c <= 0xFFFF))
				index += 0;

			// Double 16-bit character.
			else if (0x010000 <= c && c <= 0x10FFFF)
				index += 1;

			else
			{
				out_length = 0;
				out_size = 0;

				// Invalid UTF-16.
				LF_ASSERT_RETURN(false, false);
			}
		}

		out_length = length;
		out_size = index;

		return true;
	}

	SizeT StringCalls_UTF16::GetCharacterWidth(const wchar_t* utf16_character)
	{
		int32 codepoint = (int32)(utf16_character[0]);

		// Single 16-bit character. Same as UCS-2.
		if ((0x0000 <= codepoint && codepoint <= 0xD7FF) || (0xE000 <= codepoint && codepoint <= 0xFFFF))
			return 1;

		// Double 16-bit character.
		else
			return 2;
	}

	SizeT StringCalls_ASCII::Length(const char* ascii_string)
	{
		const char* ptr = ascii_string;
		while (*(ptr++));
		return ptr - ascii_string - 1;
	}

	bool StringCalls_ASCII::LengthAndSize(const char* ascii_string, SizeT& out_length, SizeT& out_size)
	{
		out_length = Length(ascii_string);
		out_size = out_length;
		return true;
	}

}