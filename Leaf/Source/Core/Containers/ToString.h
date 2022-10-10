// Copyright to Avram Traian. 2022 - 2022.
// File created on August 25 2022.

#pragma once

namespace Leaf {

	template<typename T, typename Encoding>
	struct ToString
	{
	public:
		static void Get(const T& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			// Specialize ToString<T, Encoding> in order to format this type.
			LF_ASSERT(false);
		}
	};

	namespace Utils {

		template<typename Encoding>
		static bool ParseStringAsUnsignedInteger(StringViewBase<Encoding> string, SizeT& out_value)
		{
			out_value = 0;

			SizeT index = 0;
			for (SizeT offset = 0; offset < string.Length(); offset++)
			{
				int32 codepoint;
				uint8 advance;
				Encoding::StringCallsClass::BytesToCodepoint(string.CStr() + index, codepoint, advance);
				index += advance;

				if ('0' > codepoint || codepoint > '9')
					return false;

				out_value *= 10;
				out_value += (codepoint - '0');
			}

			return true;
		}

	}

	template<typename Encoding>
	struct ToString<uint64, Encoding>
	{
	public:
		static void Get(const uint64& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			using Char = Encoding::CharacterType;

			uint8 min_digits_count = 1;

			// Flags parsing.
			if (!flags.IsEmpty())
			{
				switch (flags.CStr()[0])
				{
					case '/':
					{
						break;
					}
					case ',':
					{
						SizeT mdg;
						if (Utils::ParseStringAsUnsignedInteger(StringViewBase<Encoding>(flags.CStr() + 1, flags.Length() - 1), mdg))
							min_digits_count = (uint8)(mdg);
						else
							return;

						break;
					}
					default:
					{
						return;
					}
				}
			}

			Char result_data[64] = {};
			uint8 result_offset = 0;

			uint8 digits[32] = {};
			uint8 digits_count = 0;

			uint64 v = value;

			while (v != 0)
			{
				digits[digits_count++] = v % 10;
				v /= 10;
			}

			for (int16 index = 0; index < min_digits_count - digits_count; index++)
				result_data[result_offset++] = '0';

			for (uint8 index = 0; index < digits_count; index++)
				result_data[result_offset++] = '0' + digits[digits_count - index - 1];

			// The result is composed only from ASCII-equivalent codepoints.
			// So, we can use the offset as its length, since it has a fixed width (of 1) on all available encodings.
			out_result.Append(StringViewBase<Encoding>(result_data, result_offset));
		}
	};

	template<typename Encoding>
	struct ToString<int64, Encoding>
	{
	public:
		static void Get(const int64& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			using Char = Encoding::CharacterType;

			int64 v = value;
			if (v < 0)
			{
				v = -v;

				Char c = '-';
				out_result.AppendChar(&c, 1);
			}

			ToString<uint64, Encoding>::Get((uint64)v, out_result, flags);
		}
	};

#define LF_DECL_INTEGER_TO_STRING(TYPE, CAST_TYPE)                                                            \
	template<typename Encoding>                                                                               \
	struct ToString<TYPE, Encoding>                                                                           \
	{                                                                                                         \
	public:                                                                                                   \
		static void Get(const TYPE& v, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {}) \
		{                                                                                                     \
			ToString<CAST_TYPE, Encoding>::Get((CAST_TYPE)v, out_result, flags);                              \
		}                                                                                                     \
	}

	LF_DECL_INTEGER_TO_STRING(uint8,  uint64);
	LF_DECL_INTEGER_TO_STRING(uint16, uint64);
	LF_DECL_INTEGER_TO_STRING(uint32, uint64);

	LF_DECL_INTEGER_TO_STRING(int8,   int64);
	LF_DECL_INTEGER_TO_STRING(int16,  int64);
	LF_DECL_INTEGER_TO_STRING(int32,  int64);

	template<typename Encoding>
	struct ToString<double, Encoding>
	{
	public:
		static void Get(const double& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			using Char = Encoding::CharacterType;

			double v = value;
			if (v < 0.0)
			{
				v *= -1.0;

				Char c = '-';
				out_result.AppendChar(&c, 1);
			}

			int16 exponent = 0;

			constexpr double ScientificMaxThreshold = 1e8;
			constexpr double ScientificMinThreshold = 1e-8;

			if (v >= ScientificMaxThreshold)
			{
				if (v >= 1e256) { v *= (1.0 / 1e256); exponent += 256; }
				if (v >= 1e128) { v *= (1.0 / 1e128); exponent += 128; }
				if (v >= 1e64 ) { v *= (1.0 / 1e64);  exponent += 64;  }
				if (v >= 1e32 ) { v *= (1.0 / 1e32);  exponent += 32;  }
				if (v >= 1e16 ) { v *= (1.0 / 1e16);  exponent += 16;  }
				if (v >= 1e8  ) { v *= (1.0 / 1e8);   exponent += 8;   }
				if (v >= 1e4  ) { v *= (1.0 / 1e4);   exponent += 4;   }
				if (v >= 1e2  ) { v *= (1.0 / 1e2);   exponent += 2;   }
				if (v >= 1e1  ) { v *= (1.0 / 1e1);   exponent += 1;   }
			}
			else if (v <= ScientificMinThreshold)
			{
				if (v <= 1e-256) { v *= 1e256; exponent -= 256; }
				if (v <= 1e-128) { v *= 1e128; exponent -= 128; }
				if (v <= 1e-64)  { v *= 1e64;  exponent -= 64; }
				if (v <= 1e-32)  { v *= 1e32;  exponent -= 32; }
				if (v <= 1e-16)  { v *= 1e16;  exponent -= 16; }
				if (v <= 1e-8)   { v *= 1e8;   exponent -= 8; }
				if (v <= 1e-4)   { v *= 1e4;   exponent -= 4; }
				if (v <= 1e-2)   { v *= 1e2;   exponent -= 2; }
				if (v <= 1e-1)   { v *= 1e1;   exponent -= 1; }
			}

			uint8 decimal_precision = 3;

			if (!flags.IsEmpty())
			{
				switch (flags.CStr()[0])
				{
					case '/':
					{
						break;
					}
					case '.':
					{
						SizeT dp;
						if (Utils::ParseStringAsUnsignedInteger(StringViewBase<Encoding>(flags.CStr() + 1, flags.Length() - 1), dp))
							decimal_precision = (uint8)(dp);
						else
							return;

						break;
					}
					default:
					{
						return;
					}
				}
			}

			double decimal_multiplier = 1.0;
			for (uint8 index = 0; index < decimal_precision; index++)
				decimal_multiplier *= 10.0;

			uint64 integral = (uint64)value;
			uint64 decimal = (uint64)((value - (double)integral) * decimal_multiplier);

			ToString<uint64, Encoding>::Get(integral, out_result);

			if (decimal > 0)
			{
				Char c = '.';
				out_result.AppendChar(&c, 1);
				ToString<uint64, Encoding>::Get(decimal, out_result);
			}

			if (exponent != 0)
			{
				Char c = 'e';
				out_result.AppendChar(&c, 1);
				ToString<int16, Encoding>::Get(exponent, out_result);
			}
		}
	};

	template<typename Encoding>
	struct ToString<float, Encoding>
	{
	public:
		static void Get(const float& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<double, Encoding>::Get((double)(value), out_result, flags);
		}
	};

	template<typename Encoding>
	struct ToString<void*, Encoding>
	{
	public:
		static void Get(void* const& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			// 16 on 64-bit architectures; 8 on 32-bit.
			constexpr SizeT PointerDigitsCount = sizeof(void*) * 2;

			using Char = Encoding::CharacterType;

			// Since we store only ASCII-equivalent codepoints, we will only have fixed-width (of 1) characters.
			Char buffer[PointerDigitsCount] = {};

			uint64 v = (uint64)(value);
			for (SizeT index = 0; index < PointerDigitsCount; index++)
			{
				uint8 digit = v % 16;
				v /= 16;

				if (digit <= 9)
					buffer[PointerDigitsCount - index - 1] = '0' + digit;
				else
					buffer[PointerDigitsCount - index - 1] = 'A' + (digit - 10);
			}

			out_result.Append(StringViewBase<Encoding>(buffer, PointerDigitsCount));
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////// UTF-8 Strings ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Encoding>
	struct ToString<StringViewUTF8, Encoding>
	{
	public:
		static void Get(const StringViewUTF8& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			SizeT index = 0;
			for (SizeT offset = 0; offset < value.Length(); offset++)
			{
				int32 codepoint;
				uint8 advance;
				StringCalls_UTF8::BytesToCodepoint(value.CStr() + index, codepoint, advance);
				index += advance;

				using Char = Encoding::CharacterType;
				Char buffer[8] = {};
				uint8 character_width;
				Encoding::StringCallsClass::CodepointToBytes(codepoint, buffer, character_width);
				out_result.AppendChar(buffer, character_width);
			}
		}
	};

	template<typename Encoding>
	class StringBase;
	using StringUTF8 = StringBase<UTF8Encoding>;

	template<typename Encoding>
	struct ToString<StringUTF8, Encoding>
	{
	public:
		static void Get(const StringUTF8& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF8, Encoding>::Get(StringViewUTF8(value), out_result, flags);
		}
	};

	template<typename Encoding>
	struct ToString<const char*, Encoding>
	{
	public:
		static void Get(const char* const& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF8, Encoding>::Get(StringViewUTF8(value), out_result, flags);
		}
	};

	template<typename Encoding>
	struct ToString<char*, Encoding>
	{
	public:
		static void Get(char* const& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF8, Encoding>::Get(StringViewUTF8(value), out_result, flags);
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////// UTF-16 Strings //////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Encoding>
	struct ToString<StringViewUTF16, Encoding>
	{
	public:
		static void Get(const StringViewUTF16& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			SizeT index = 0;
			for (SizeT offset = 0; offset < value.Length(); offset++)
			{
				int32 codepoint;
				uint8 advance;
				StringCalls_UTF16::BytesToCodepoint(value.CStr() + index, codepoint, advance);
				index += advance;

				using Char = Encoding::CharacterType;
				Char buffer[8] = {};
				uint8 character_width;
				Encoding::StringCallsClass::CodepointToBytes(codepoint, buffer, character_width);
				out_result.AppendChar(buffer, character_width);
			}
		}
	};

	template<typename Encoding>
	class StringBase;
	using StringUTF16 = StringBase<UTF16Encoding>;

	template<typename Encoding>
	struct ToString<StringUTF16, Encoding>
	{
	public:
		static void Get(const StringUTF16& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF16, Encoding>::Get(StringViewUTF16(value), out_result, flags);
		}
	};

	template<typename Encoding>
	struct ToString<const wchar_t*, Encoding>
	{
	public:
		static void Get(const wchar_t* const& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF16, Encoding>::Get(StringViewUTF16(value), out_result, flags);
		}
	};

	template<typename Encoding>
	struct ToString<wchar_t*, Encoding>
	{
	public:
		static void Get(wchar_t* const& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewUTF16, Encoding>::Get(StringViewUTF16(value), out_result, flags);
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////// ASCII Strings ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename Encoding>
	struct ToString<StringViewASCII, Encoding>
	{
	public:
		static void Get(const StringViewASCII& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			SizeT index = 0;
			for (SizeT offset = 0; offset < value.Length(); offset++)
			{
				int32 codepoint;
				uint8 advance;
				StringCalls_ASCII::BytesToCodepoint(value.CStr() + index, codepoint, advance);
				index += advance;

				using Char = Encoding::CharacterType;
				Char buffer[8] = {};
				uint8 character_width;
				Encoding::StringCallsClass::CodepointToBytes(codepoint, buffer, character_width);
				out_result.AppendChar(buffer, character_width);
			}
		}
	};

	template<typename Encoding>
	class StringBase;
	using StringASCII = StringBase<ASCIIEncoding>;

	template<typename Encoding>
	struct ToString<StringASCII, Encoding>
	{
	public:
		static void Get(const StringASCII& value, StringBase<Encoding>& out_result, StringViewBase<Encoding> flags = {})
		{
			ToString<StringViewASCII, Encoding>::Get(StringViewASCII(value), out_result, flags);
		}
	};

}