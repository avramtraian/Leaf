// Copyright to Avram Traian. 2022 - 2022.
// File created on August 25 2022.

#pragma once

#include "StringCalls.h"

namespace Leaf {

	template<typename T, typename CharType, typename AllocatorType>
	struct ToString
	{
	public:
		/*
		static void Get(BasicString<CharType, AllocatorType>& out_result, const T& value, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			out_result.Append(OBJECT_AS_STRING);
		}
		*/
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<uint64, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const uint64& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			CharType result_data[64] = {};
			uint8 result_offset = 0;

			uint8 digits[32] = {};
			uint8 digits_count = 0;

			uint64 value = v;

			while (value != 0)
			{
				digits[digits_count++] = value % 10;
				value /= 10;
			}

			constexpr uint8 DefaultMinDigitsCount = 1;
			uint8 min_digits_count = 0;
			bool use_default_flags = true;

			if (!flags.IsEmpty())
			{
				switch (flags.CStr()[0])
				{
					case ',':
					{
						use_default_flags = flags.Length() <= 1;

						for (SizeT index = 1; index < flags.Length(); index++)
						{
							if ('9' < flags.CStr()[index] || flags.CStr()[index] < '0')
							{
								out_result.Append("@@INVALID_FLAGS@@");
								return;
							}

							min_digits_count *= 10;
							min_digits_count += flags.CStr()[index] - '0';
						}
						break;
					}
					default:
					{
						out_result.Append("@@INVALID_FLAGS@@");
						return;
					}
				}
			}

			if (use_default_flags)
				min_digits_count = DefaultMinDigitsCount;

			for (int16 index = 0; index < min_digits_count - digits_count; index++)
				result_data[result_offset++] = '0';

			for (uint8 index = 0; index < digits_count; index++)
			{
				result_data[result_offset++] = '0' + digits[digits_count - index - 1];
			}

			out_result.Append(BasicStringView<CharType>(result_data, result_offset));
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<int64, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const int64& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			int64 value = v;
			if (v < 0)
			{
				out_result.AppendChar('-');
				value *= -1;
			}

			ToString<uint64, CharType, AllocatorType>::Get(out_result, (uint64)value, flags);
		}
	};

#define LF_DECL_INTEGER_TO_STRING(TYPE, CAST_TYPE)                                                                                                      \
	template<typename CharType, typename AllocatorType>                                                                                                 \
	struct ToString<TYPE, CharType, AllocatorType>                                                                                                      \
	{                                                                                                                                                   \
	public:                                                                                                                                             \
		static void Get(BasicString<CharType, AllocatorType>& out_result, const TYPE& v, BasicStringView<CharType> flags = BasicStringView<CharType>()) \
		{                                                                                                                                               \
			ToString<CAST_TYPE, CharType, AllocatorType>::Get(out_result, (CAST_TYPE)v, flags);                                                         \
		}                                                                                                                                               \
	}

	LF_DECL_INTEGER_TO_STRING(uint8,  uint64);
	LF_DECL_INTEGER_TO_STRING(uint16, uint64);
	LF_DECL_INTEGER_TO_STRING(uint32, uint64);

	LF_DECL_INTEGER_TO_STRING(int8,   int64);
	LF_DECL_INTEGER_TO_STRING(int16,  int64);
	LF_DECL_INTEGER_TO_STRING(int32,  int64);

	template<typename CharType, typename AllocatorType>
	struct ToString<double, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const double& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			double value = v;
			if (value < 0.0)
			{
				value *= -1.0;
				out_result.AppendChar('-');
			}

			int16 exponent = 0;

			constexpr double ScientificMaxThreshold = 1e8;
			constexpr double ScientificMinThreshold = 1e-8;

			if (value >= ScientificMaxThreshold)
			{
				if (value >= 1e256) { value *= (1.0 / 1e256); exponent += 256; }
				if (value >= 1e128) { value *= (1.0 / 1e128); exponent += 128; }
				if (value >= 1e64 ) { value *= (1.0 / 1e64);  exponent += 64;  }
				if (value >= 1e32 ) { value *= (1.0 / 1e32);  exponent += 32;  }
				if (value >= 1e16 ) { value *= (1.0 / 1e16);  exponent += 16;  }
				if (value >= 1e8  ) { value *= (1.0 / 1e8);   exponent += 8;   }
				if (value >= 1e4  ) { value *= (1.0 / 1e4);   exponent += 4;   }
				if (value >= 1e2  ) { value *= (1.0 / 1e2);   exponent += 2;   }
				if (value >= 1e1  ) { value *= (1.0 / 1e1);   exponent += 1;   }
			}
			else if (value <= ScientificMinThreshold)
			{
				if (value <= 1e-256) { value *= 1e256; exponent -= 256; }
				if (value <= 1e-128) { value *= 1e128; exponent -= 128; }
				if (value <= 1e-64)  { value *= 1e64;  exponent -= 64; }
				if (value <= 1e-32)  { value *= 1e32;  exponent -= 32; }
				if (value <= 1e-16)  { value *= 1e16;  exponent -= 16; }
				if (value <= 1e-8)   { value *= 1e8;   exponent -= 8; }
				if (value <= 1e-4)   { value *= 1e4;   exponent -= 4; }
				if (value <= 1e-2)   { value *= 1e2;   exponent -= 2; }
				if (value <= 1e-1)   { value *= 1e1;   exponent -= 1; }
			}

			constexpr uint8 DefaultDecimalPrecision = 3;
			uint8 decimal_precision = 0;
			bool use_default_flags = true;

			if (!flags.IsEmpty())
			{
				switch (flags.CStr()[0])
				{
					case '.':
					{
						use_default_flags = flags.Length() <= 1;

						for (SizeT index = 1; index < flags.Length(); index++)
						{
							if ('9' < flags.CStr()[index] || flags.CStr()[index] < '0')
							{
								out_result.Append("@@INVALID_FLAGS@@");
								return;
							}

							decimal_precision *= 10;
							decimal_precision += flags.CStr()[index] - '0';
						}

						break;
					}
					default:
					{
						out_result.Append("@@INVALID_FLAGS@@");
						return;
					}
				}
			}

			if (use_default_flags)
				decimal_precision = DefaultDecimalPrecision;

			double decimal_multiplier = 1.0;
			for (uint8 index = 0; index < decimal_precision; index++)
				decimal_multiplier *= 10.0;

			uint64 integral = (uint64)value;
			uint64 decimal = (uint64)((value - (double)integral) * decimal_multiplier);

			ToString<uint64, CharType, AllocatorType>::Get(out_result, integral);

			if (decimal > 0)
			{
				out_result.AppendChar('.');
				ToString<uint64, CharType, AllocatorType>::Get(out_result, decimal);
			}

			if (exponent != 0)
			{
				out_result.AppendChar('e');
				ToString<int16, CharType, AllocatorType>::Get(out_result, exponent);
			}
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<float, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const float& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			ToString<double, CharType, AllocatorType>::Get(out_result, (double)v, flags);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<BasicStringView<char>, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const BasicStringView<char>& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			SizeT result_length = out_result.Length();

			out_result.SetMaxSize(result_length + v.Length() + 1);

			CharType* result_data = out_result.Data();

			for (SizeT index = 0; index < v.Length(); index++)
				result_data[result_length++] = (CharType)v.Data()[index];
			result_data[result_length] = 0;

			out_result.SetSizeInternal(result_length + 1);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<BasicString<char, AllocatorType>, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const BasicString<char, AllocatorType>& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			ToString<BasicStringView<char>, CharType, AllocatorType>::Get(out_result, v.ToView(), flags);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<const char*, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const char* const& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			ToString<BasicStringView<char>, CharType, AllocatorType>::Get(out_result, v, flags);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<BasicStringView<wchar_t>, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const BasicStringView<wchar_t>& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			SizeT result_length = out_result.Length();

			out_result.SetMaxSize(result_length + v.Length() + 1);

			CharType* result_data = out_result.Data();

			for (SizeT index = 0; index < v.Length(); index++)
				result_data[result_length++] = (CharType)v.Data()[index];
			result_data[result_length] = 0;

			out_result.SetSizeInternal(result_length + 1);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<BasicString<wchar_t, AllocatorType>, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const BasicString<wchar_t, AllocatorType>& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			ToString<BasicStringView<wchar_t>, CharType, AllocatorType>::Get(out_result, v.ToView(), flags);
		}
	};

	template<typename CharType, typename AllocatorType>
	struct ToString<const wchar_t*, CharType, AllocatorType>
	{
	public:
		static void Get(BasicString<CharType, AllocatorType>& out_result, const wchar_t* const& v, BasicStringView<CharType> flags = BasicStringView<CharType>())
		{
			ToString<BasicStringView<wchar_t>, CharType, AllocatorType>::Get(out_result, v, flags);
		}
	};

}