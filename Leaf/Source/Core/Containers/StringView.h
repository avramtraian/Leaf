// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "StringCalls.h"

namespace Leaf {

	template<typename CharType, typename AllocatorType>
	class BasicString;

	enum class SearchDirection : uint8
	{
		FromStart, FromEnd
	};

	enum class SearchCase : uint8
	{
		CaseSensitive, IgnoreCase
	};

	template<typename CharType>
	class BasicStringView
	{
	public:
		static constexpr uint64 InvalidPos = static_cast<uint64>(-1);

	public:
		BasicStringView()
			: m_String(nullptr)
			, m_Length(0)
		{}

		BasicStringView(const BasicStringView& other)
			: m_String(other.m_String)
			, m_Length(other.m_Length)
		{}

		BasicStringView(const CharType* string)
			: m_String(string)
			, m_Length(StringCalls_Templated::Length<CharType>(string))
		{}

		BasicStringView(const CharType* string, uint64 length)
			: m_String(string)
			, m_Length(length)
		{}

		~BasicStringView()
		{}

	public:
		const CharType* Data() const { return m_String; }

		const CharType* CStr() const { return m_String; }

		uint64 Length() const { return m_Length; }

		bool IsEmpty() const { return m_Length == 0; }

	public:
		uint64 Find(const BasicStringView& substring, uint64 start = 0, uint64 end = 0, SearchDirection direction = SearchDirection::FromStart, SearchCase scase = SearchCase::CaseSensitive) const
		{
			if (direction == SearchDirection::FromStart)
			{
				if (scase == SearchCase::CaseSensitive)
					return Find_FromStart_CaseSensitive(substring, start, end);
				else
					return Find_FromStart_IgnoreCase(substring, start, end);
			}
			else
			{
				if (scase == SearchCase::CaseSensitive)
					return Find_FromEnd_CaseSensitive(substring, start, end);
				else
					return Find_FromEnd_IgnoreCase(substring, start, end);
			}
		}

		uint64 Find_FromStart_CaseSensitive(const BasicStringView& substring, uint64 start = 0, uint64 end = 0) const
		{
			if (end == 0)
				end = m_Length;

			if (substring.m_Length > end - start)
				return InvalidPos;

			for (uint64 index = start; index <= (end - start) - substring.m_Length; index++)
			{
				bool found = true;
				for (uint64 sub_index = 0; sub_index < substring.m_Length; sub_index++)
				{
					if (m_String[index + sub_index] != substring.m_String[sub_index])
					{
						found = false;
						break;
					}
				}
				if (found)
					return index;
			}

			return InvalidPos;
		}

		uint64 Find_FromStart_IgnoreCase(const BasicStringView& substring, uint64 start = 0, uint64 end = 0) const
		{
			if (end == 0)
				end = m_Length;

			if (substring.m_Length > end - start)
				return InvalidPos;

			for (uint64 index = start; index <= (end - start) - substring.m_Length; index++)
			{
				bool found = true;
				for (uint64 sub_index = 0; sub_index < substring.m_Length; sub_index++)
				{
					if (!CompareChars_IgnoreCase(m_String[index + sub_index], substring.m_String[sub_index]))
					{
						found = false;
						break;
					}
				}
				if (found)
					return index;
			}

			return InvalidPos;
		}

		uint64 Find_FromEnd_CaseSensitive(const BasicStringView& substring, uint64 start = 0, uint64 end = 0) const
		{
			if (end == 0)
				end = m_Length;

			if (substring.m_Length > end - start)
				return InvalidPos;

			for (int64 index = (end - start) - substring.m_Length; index >= 0; index--)
			{
				bool found = true;
				for (uint64 sub_index = 0; sub_index < substring.m_Length; sub_index++)
				{
					if (m_String[index + sub_index] != substring.m_String[sub_index])
					{
						found = false;
						break;
					}
				}
				if (found)
					return index;
			}

			return InvalidPos;
		}

		uint64 Find_FromEnd_IgnoreCase(const BasicStringView& substring, uint64 start = 0, uint64 end = 0) const
		{
			if (end == 0)
				end = m_Length;

			if (substring.m_Length > end - start)
				return InvalidPos;

			for (int64 index = (end - start) - substring.m_Length; index >= 0; index--)
			{
				bool found = true;
				for (uint64 sub_index = 0; sub_index < substring.m_Length; sub_index++)
				{
					if (!CompareChars_IgnoreCase(m_String[index + sub_index], substring.m_String[sub_index]))
					{
						found = false;
						break;
					}
				}
				if (found)
					return index;
			}

			return InvalidPos;
		}

		BasicStringView Substring(uint64 start, uint64 end = 0) const
		{
			if (end == 0)
				end = m_Length;

			return BasicStringView(m_String + start, end - start);
		}

	private:
		static constexpr bool CompareChars_IgnoreCase(CharType ca, CharType cb)
		{
			uint64 a = (uint64)ca;
			uint64 b = (uint64)cb;

			if (a == b)
				return true;

			if ('a' <= a && a <= 'z')
				a -= (uint64)('a' - 'A');
			else
				return false;

			return (a == b);
		}

	private:
		const CharType* m_String;
		uint64 m_Length;

	private:
		template<typename FriendCharType, typename FriendAllocatorType>
		friend class BasicString;
	};

	using StringView = BasicStringView<char>;

}