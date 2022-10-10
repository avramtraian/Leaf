// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "StringCalls.h"

namespace Leaf {

	template<typename Encoding>
	class StringBase;

	/**
	* 
	*/
	template<typename Encoding>
	class StringViewBase
	{
	public:
		static constexpr SizeT InvalidPosition = SIZE_MAX;

	private:
		/**  */
		using Char = typename Encoding::CharacterType;

		/**  */
		using StringCalls = typename Encoding::StringCallsClass;

		/**  */
		using String = StringBase<Encoding>;

	public:
		StringViewBase()
			: m_Data(nullptr)
			, m_Length(0)
		{}

		StringViewBase(const StringViewBase& other)
			: m_Data(other.m_Data)
			, m_Length(other.m_Length)
		{}

		StringViewBase(StringViewBase&& other) noexcept
			: m_Data(Leaf::Move(other.m_Data))
			, m_Length(Leaf::Move(other.m_Length))
		{
			other.m_Data = nullptr;
			other.m_Length = 0;
		}

		StringViewBase(const Char* string)
			: m_Data(string)
			, m_Length(StringCalls::Length(string))
		{}

		StringViewBase(const Char* string, SizeT string_length)
			: m_Data(string)
			, m_Length(string_length)
		{}

		/**
		* Defined in 'String.h'.
		*/
		StringViewBase(const String& string);

		~StringViewBase()
		{}

	public:
		StringViewBase& operator=(const StringViewBase& other)
		{
			m_Data = other.m_Data;
			m_Length = other.m_Length;

			return *this;
		}

		StringViewBase& operator=(StringViewBase&& other) noexcept
		{
			m_Data = Leaf::Move(other.m_Data);
			m_Length = Leaf::Move(other.m_Length);

			other.m_Data = nullptr;
			other.m_Length = 0;

			return *this;
		}

		StringViewBase& operator=(const Char* string)
		{
			m_Data = string;
			m_Length = StringCalls::Length(string);

			return *this;
		}

		/**
		* Defined in 'String.h'.
		*/
		StringViewBase& operator=(const String& string);

	public:
		const Char* Data() const { return m_Data; }

		const Char* CStr() const { return m_Data; }

		SizeT Length() const { return m_Length; }

		bool IsEmpty() const { return (m_Length == 0); }

	public:
		SizeT Size() const
		{
			SizeT string_size = 0;
			for (SizeT index = 0; index < m_Length; index++)
				string_size += StringCalls::GetCharacterWidth(m_Data + string_size);

			return string_size;
		}

		/**
		* Finds the first instance of the provided substring.
		* 
		* @param substring The sequence to be searched.
		* @param starting_offset The offset (in characters) from which the search will begin.
		* @param search_length The length (in characters) of the sequence where the search will be performed.
		*			If this is set to the default value, 'SIZE_MAX', it will search until the end of the string.
		* 
		* @returns The offset (in characters), from the beginning, where the first instance is located, if present.
		*			If there is no instance found, it will return 'InvalidPosition'. If the parameters passed are
		*			invalid and/or in a bad combination, it will also return 'InvalidPosition'.
		*/
		SizeT FindFirstOf(StringViewBase substring, SizeT starting_offset = 0, SizeT search_length = SIZE_MAX)
		{
			if (search_length == SIZE_MAX)
				search_length = m_Length - starting_offset;

			if (substring.Length() > search_length)
				return InvalidPosition;

			SizeT substring_size = substring.Size();
			SizeT starting_index = StringViewBase(m_Data, starting_offset).Size();
			SizeT search_size = StringViewBase(m_Data + starting_index, search_length).Size();

			if (substring_size > search_size)
				return InvalidPosition;

			LF_ASSERT_RETURN(starting_index + search_size > Size(), InvalidPosition);

			for (SizeT index = starting_index; index < starting_index + search_size - substring_size; index++)
			{
				bool match = true;

				for (SizeT substring_index = 0; substring_index < substring_size; substring_index++)
				{
					if (m_Data[index + substring_index] == substring.Data()[substring_index])
					{
						match = false;
						break;
					}
				}

				if (match)
					return index;
			}

			return InvalidPosition;
		}

		/**
		* 
		*/
		StringViewBase Substring(SizeT starting_offset, SizeT substring_length)
		{
			LF_ASSERT_RETURN(starting_offset + substring_length > m_Length, StringViewBase());

			SizeT starting_index = StringViewBase(m_Data, starting_offset).Size();
			return StringViewBase(m_Data + starting_index, substring_length);
		}

	private:
		/**  */
		const Char* m_Data;

		/**  */
		SizeT m_Length;
	};

	/**  */
	using StringViewUTF8  = StringViewBase<UTF8Encoding>;

	/**  */
	using StringViewUTF16 = StringViewBase<UTF16Encoding>;

	/**  */
	using StringViewASCII = StringViewBase<ASCIIEncoding>;

}