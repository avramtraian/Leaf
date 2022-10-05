// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "StringCalls.h"
#include "Vector.h"
#include "StringView.h"
#include "ToString.h"

namespace Leaf {

	/**
	* 
	*/
	template<typename Encoding>
	class StringBase
	{
	private:
		/**  */
		using Char = typename Encoding::CharacterType;

		/**  */
		using StringCalls = typename Encoding::StringCallsClass;

		/**  */
		using StringView = StringViewBase<Encoding>;

	public:
		StringBase()
		{
			m_Base.Add(0);
			m_Length = 0;
		}

		StringBase(const StringBase& other)
			: m_Base(other.m_Base)
			, m_Length(other.m_Length)
		{}

		StringBase(StringBase&& other) noexcept
			: m_Base(Leaf::Move(other.m_Base))
			, m_Length(Leaf::Move(other.m_Length))
		{
			other.m_Base.Add(0);
			other.m_Length = 0;
		}

		StringBase(const Char* string)
		{
			SizeT string_size;
			StringCalls::Length(string, m_Length, string_size);

			m_Base.SetSizeUninitialized(string_size + 1);
			Memory::Copy(m_Base.Data(), string, m_Base.Size() * sizeof(Char));
		}

		StringBase(const Char* string, SizeT string_length)
		{
			SizeT string_size = 0;
			for (SizeT index = 0; index < string_length; index++)
				string_size += StringCalls::GetCharacterWidth(string + string_size);

			m_Base.SetSizeUninitialized(string_size + 1);
			Memory::Copy(m_Base.Data(), string, string_size * sizeof(Char));
			m_Base.Back() = 0;
		}

		StringBase(StringView string)
			: m_Length(string.Length())
		{
			SizeT string_size = string.Size();

			m_Base.SetSizeUninitialized(string_size + 1);
			Memory::Copy(m_Base.Data(), string.Data(), string_size * sizeof(char));
			m_Base.Back() = 0;
		}

		~StringBase()
		{
			// Reduces the Vector's destructor overhead.
			m_Base.SetSizeInternal(0);
		}

	public:
		StringBase& operator=(const StringBase& other)
		{
			// Reduces the Vector's destructor overhead.
			m_Base.SetSizeInternal(0);

			m_Base = other.m_Base;
			m_Length = other.m_Length;

			return *this;
		}

		StringBase& operator=(StringBase&& other) noexcept
		{
			// Reduces the Vector's destructor overhead.
			m_Base.SetSizeInternal(0);

			m_Base = Leaf::Move(other.m_Base);
			m_Length = Leaf::Move(other.m_Length);

			other.m_Base.Add(0);
			other.m_Length = 0;

			return *this;
		}

		StringBase& operator=(const Char* string)
		{
			// Reduces the Vector's destructor overhead.
			m_Base.SetSizeInternal(0);

			SizeT string_size;
			StringCalls::LengthAndSize(string, m_Length, string_size);

			m_Base.SetSizeUninitialized(string_size + 1);
			Memory::Copy(m_Base.Data(), string, m_Base.Size() * sizeof(Char));

			return *this;
		}

		StringBase& operator=(StringView string)
		{
			// Reduces the Vector's destructor overhead.
			m_Base.SetSizeInternal(0);

			SizeT string_size = string.Size();

			m_Base.SetSizeUninitialized(string_size + 1);
			Memory::Copy(m_Base.Data(), string.Data(), string_size * sizeof(char));
			m_Base.Back() = 0;

			m_Length = string.Length();

			return *this;
		}

	public:
		Char* Data() const { return m_Base.Data(); }

		const Char* CStr() const { return m_Base.Data(); }

		SizeT Length() const { return m_Length; }

		SizeT Size() const { return m_Base.Size(); }

		SizeT Capacity() const { return m_Base.Capacity(); }

	public:
		StringBase& Append(StringView substring)
		{
			SizeT substring_size = substring.Size();

			SizeT current_size = m_Base.Size();
			m_Base.SetSizeUninitialized(m_Base.Size() + substring_size);

			Memory::Copy(m_Base.Data() + current_size - 1, substring.Data(), substring_size * sizeof(Char));
			m_Base.Back() = 0;

			m_Length += substring.Length();

			return *this;
		}

		StringBase& AppendChar(const Char* character)
		{
			return Append(StringView(character, 1));
		}

		StringBase& AppendChar(const Char* character, uint8 character_width)
		{
			SizeT current_size = m_Base.Size();
			m_Base.SetSizeUninitialized(m_Base.Size() + character_width);

			Memory::Copy(m_Base.Data() + current_size - 1, character, character_width * sizeof(Char));
			m_Base.Back() = 0;

			m_Length++;

			return *this;
		}

	public:
		template<typename... Args>
		static StringBase Format(StringView format, Args&&... args)
		{
			StringBase result;
			
			FormatImpl(format, result, Leaf::Forward<Args>(args)...);
			if (format.Length() > 0)
				result.Append(format);

			return result;
		}

	private:
		static void FormatImpl(StringView& format, StringBase& out_result)
		{
			out_result.Append(format);
			format = StringView(nullptr, 0);
		}

		template<typename T>
		static void FormatImpl(StringView& format, StringBase& out_result, T&& value)
		{
			SizeT index = 0;
			for (SizeT offset = 0; offset < format.Length(); offset++)
			{
				int32 codepoint;
				uint8 character_width;
				StringCalls::BytesToCodepoint(format.CStr() + index, codepoint, character_width);

				if (codepoint == '{')
				{
					SizeT flags_start_index = index + 1;
					SizeT flags_start_offset = offset + 1;

					bool is_valid = false;
					for (offset = offset; offset < format.Length(); offset++)
					{
						int32 codepoint;
						uint8 character_width;
						StringCalls::BytesToCodepoint(format.CStr() + index, codepoint, character_width);

						if (codepoint == '}')
						{
							is_valid = true;
							break;
						}

						index += character_width;
					}

					LF_ASSERT_CONTINUE(is_valid);
					StringView flags = StringView(format.CStr() + flags_start_index, offset - flags_start_offset);

					using ArgumentType =
						RemoveConstType<
							ArrayToPointerDecayType<
								RemoveReferenceType<
									T
					>>>;

					ToString<ArgumentType, Encoding>::Get(value, out_result, flags);

					format = StringView(format.CStr() + (index + 1), format.Length() - (offset + 1));
					return;
				}
				else
				{
					out_result.AppendChar(format.CStr() + index, character_width);
				}

				index += character_width;
			}

			format = StringView(nullptr, 0);
		}

		template<typename T, typename... Args>
		static void FormatImpl(StringView& format, StringBase& out_result, T&& value, Args&&... args)
		{
			FormatImpl(format, out_result, value);
			FormatImpl(format, out_result, Leaf::Forward<Args>(args)...);
		}

	private:
		/**  */
		Vector<Char> m_Base;

		/**  */
		SizeT m_Length;
	};

	/**  */
	using StringUTF8  = StringBase<UTF8Encoding>;

	/**  */
	using StringUTF16 = StringBase<UTF16Encoding>;

	/**  */
	using StringASCII = StringBase<ASCIIEncoding>;

	template<typename Encoding>
	StringViewBase<Encoding>::StringViewBase(const String& string)
		: m_Data(string.CStr())
		, m_Length(string.Length())
	{}

	template<typename Encoding>
	StringViewBase<Encoding>& StringViewBase<Encoding>::operator=(const String& string)
	{
		m_Data = string.CStr();
		m_Length = string.Length();
	}

}