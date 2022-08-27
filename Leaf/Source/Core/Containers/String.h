// Copyright to Avram Traian. 2022 - 2022.
// File created on August 22 2022.

#pragma once

#include "Allocator.h"
#include "StringView.h"
#include "ToString.h"

namespace Leaf {

	template<typename CharType, typename AllocatorType>
	class BasicString
	{
	public:
		static constexpr bool FitsOnSSO(uint64 size)
		{
			return (size <= SSOSize);
		}

	public:
		static constexpr uint64 SSOSize = (sizeof(CharType*) + sizeof(uint64)) / sizeof(CharType);

		static constexpr uint64 InvalidPos = static_cast<uint64>(-1);

	public:
		BasicString()
			: m_Size(1)
			, m_UsesSSO(true)
		{
			m_SSOData[0] = 0;
		}

		BasicString(const BasicString& other)
			: m_Size(1)
			, m_UsesSSO(true)
		{
			CopyConstructor(other.ToView());
		}

		template<typename OtherAllocatorType>
		BasicString(const BasicString<CharType, OtherAllocatorType>& other)
			: m_Size(1)
			, m_UsesSSO(true)
		{
			CopyConstructor(other.ToView());
		}

		BasicString(BasicString&& other) noexcept
			: m_Size(1)
			, m_UsesSSO(true)
		{
			MoveConstructor<AllocatorType>(Leaf::Move(other));
		}

		template<typename OtherAllocatorType>
		BasicString(BasicString<CharType, OtherAllocatorType>&& other) noexcept
			: m_Size(1)
			, m_UsesSSO(true)
		{
			MoveConstructor<OtherAllocatorType>(Leaf::Move(other));
		}

		BasicString(const BasicStringView<CharType>& view)
		{
			CopyConstructor(view);
		}

		BasicString(const CharType* string)
		{
			CopyConstructor(BasicStringView<CharType>{ string });
		}

		~BasicString()
		{
			if (!m_UsesSSO)
				FreeData();
		}

	public:
		BasicString& operator=(const BasicString& other)
		{
			CopyOperator(other.ToView());
			return *this;
		}

		template<typename OtherAllocatorType>
		BasicString& operator=(const BasicString<CharType, OtherAllocatorType>& other)
		{
			CopyOperator(other.ToView());
			return *this;
		}

		BasicString& operator=(BasicString&& other) noexcept
		{
			MoveOperator<AllocatorType>(Leaf::Move(other));
			return *this;
		}

		template<typename OtherAllocatorType>
		BasicString& operator=(BasicString<CharType, OtherAllocatorType>&& other) noexcept
		{
			MoveOperator<OtherAllocatorType>(Leaf::Move(other));
			return *this;
		}

		BasicString& operator=(const BasicStringView<CharType>& view)
		{
			CopyOperator(view);
			return *this;
		}

		BasicString& operator=(const CharType* string)
		{
			CopyOperator(BasicStringView<CharType>{ string });
			return *this;
		}

	public:
		CharType* Data() const { return m_UsesSSO ? (CharType*)(&m_SSOData[0]) : m_Data; }

		const CharType* CStr() const { return Data(); }

		uint64 Length() const { return m_Size - 1; }

		uint64 MaxSize() const { return m_UsesSSO ? SSOSize - 1 : m_Capacity - 1; }

		bool IsEmpty() const { return Length() == 0; }

	public:
		uint64 Find(const BasicStringView<CharType>& substring, uint64 start = 0, uint64 end = 0, SearchDirection direction = SearchDirection::FromStart, SearchCase scase = SearchCase::CaseSensitive) const
		{
			return ToView().Find(substring, start, end, direction, scase);
		}

		BasicString Substring(uint64 start, uint64 end) const
		{
			return BasicString(ToView().Substring(start, end));
		}

		BasicString Insert(const BasicStringView<CharType>& substring, uint64 index) const
		{
			const CharType* this_data = Data();

			BasicString result;
			CharType* result_data = nullptr;

			if (FitsOnSSO(m_Size + substring.m_Length))
			{
				result_data = result.m_SSOData;
			}
			else
			{
				result.ReAllocateNoCopy(m_Size + substring.m_Length);
				result_data = result.m_Data;
			}

			Memory::Copy(result_data, this_data, index * sizeof(CharType));
			Memory::Copy(result_data + index, substring.m_String, substring.m_Length * sizeof(CharType));
			Memory::Copy(result_data + index + substring.m_Length, this_data + index, (m_Size - index) * sizeof(CharType));
			result.m_Size = m_Size + substring.m_Length;

			return result;
		}

		void InsertSelf(const BasicStringView<CharType>& substring, uint64 index)
		{
			if (m_UsesSSO)
			{
				CharType buffer[SSOSize] = {};
				Memory::Copy(buffer, m_SSOData, m_Size * sizeof(CharType));

				if (FitsOnSSO(m_Size + substring.m_Length))
				{
					Memory::Copy(m_SSOData + index, substring.m_String, substring.m_Length * sizeof(CharType));
					Memory::Copy(m_SSOData + index + substring.m_Length, buffer + index, (m_Size - index) * sizeof(CharType));
				}
				else
				{
					ReAllocateNoCopy(m_Size + substring.m_Length);
					Memory::Copy(m_Data, buffer, index * sizeof(CharType));
					Memory::Copy(m_Data + index, substring.m_String, substring.m_Length * sizeof(CharType));
					Memory::Copy(m_Data + index + substring.m_Length, buffer + index, (m_Size - index) * sizeof(CharType));
				}
			}
			else
			{
				if (m_Size + substring.m_Length > m_Capacity)
					ReAllocate(m_Size + substring.m_Length);

				for (int64 i = m_Size + substring.m_Length - 1; i >= m_Size; i--)
				{
					m_Data[i] = m_Data[i - substring.m_Length];
				}
				for (uint64 i = 0; i < substring.m_Length; i++)
				{
					m_Data[index + i] = substring.m_String[i];
				}
			}

			m_Size += substring.m_Length;
		}

		BasicString& Append(BasicStringView<CharType> substring)
		{
			CharType* this_data = nullptr;

			if (m_UsesSSO)
			{
				if (FitsOnSSO(m_Size + substring.m_Length))
				{
					this_data = m_SSOData;
				}
				else
				{
					ReAllocate(m_Size + substring.m_Length);
					this_data = m_Data;
				}
			}
			else
			{
				if (m_Size + substring.m_Length > m_Capacity)
					ReAllocate(m_Size + substring.m_Length);

				this_data = m_Data;
			}

			Memory::Copy(this_data + m_Size - 1, substring.m_String, substring.m_Length * sizeof(CharType));
			this_data[m_Size + substring.m_Length - 1] = 0;
			m_Size += substring.m_Length;

			return *this;
		}

		BasicString& AppendChar(CharType c)
		{
			CharType* this_data = nullptr;

			if (m_UsesSSO)
			{
				if (FitsOnSSO(m_Size + 1))
				{
					this_data = m_SSOData;
				}
				else
				{
					ReAllocate(m_Size + 1);
					this_data = m_Data;
				}
			}
			else
			{
				if (m_Size + 1 > m_Capacity)
					ReAllocate(m_Size + 1);

				this_data = m_Data;
			}

			this_data[m_Size - 1] = c;
			this_data[m_Size] = 0;
			m_Size++;

			return *this;
		}

	public:
		template<typename... Args>
		static BasicString Format(BasicStringView<CharType> format, Args&&... args)
		{
			BasicString result;
			FormatI(format, result, Leaf::Forward<Args>(args)...);
			if (format.m_Length > 0)
				result.Append(format);

			return result;
		}

	public:
		void SetMaxSize(SizeT new_max_size)
		{
			if (m_UsesSSO)
			{
				if (FitsOnSSO(new_max_size))
					return;

				ReAllocate(new_max_size);
			}
			else
			{
				if (m_Capacity >= new_max_size)
					return;

				ReAllocate(new_max_size);
			}
		}

		void SetSizeInternal(SizeT new_size)
		{
			m_Size = new_size;
		}

		void Clear()
		{
			m_Size = 1;
			Data()[0] = 0;
		}

		void Shrink()
		{
			if (m_UsesSSO)
				return;

			if (FitsOnSSO(m_Size))
			{
				CharType buffer[SSOSize] = {};
				Memory::Copy(buffer, m_Data, m_Size * sizeof(CharType));
				FreeData();

				Memory::Copy(m_SSOData, buffer, m_Size * sizeof(CharType));
				m_UsesSSO = true;
			}
			else if (m_Size != m_Capacity)
			{
				ReAllocate(m_Size);
			}
		}

		void ClearAndShrink()
		{
			if (!m_UsesSSO)
				FreeData();

			m_Size = 1;
			m_SSOData[0] = 0;
			m_UsesSSO = true;
		}

	public:
		BasicStringView<CharType> ToView() const
		{
			return BasicStringView<CharType>(Data(), Length());
		}

	private:
		void CopyConstructor(const BasicStringView<CharType>& other)
		{
			uint64 other_size = other.Length() + 1;
			const CharType* other_data = other.Data();

			if (other_size <= 1)
			{
				m_SSOData[0] = 0;
				return;
			}

			if (FitsOnSSO(other_size))
			{
				Memory::Copy(m_SSOData, other_data, (other_size - 1) * sizeof(CharType));
				m_SSOData[other_size - 1] = 0;
				m_Size = other_size;
				m_UsesSSO = true;
			}
			else
			{
				ReAllocateNoCopy(other_size);
				Memory::Copy(m_Data, other_data, (other_size - 1) * sizeof(CharType));
				m_Data[other_size - 1] = 0;
				m_Size = other_size;
				m_UsesSSO = false;
			}
		}

		template<typename OtherAllocatorType>
		void MoveConstructor(BasicString<CharType, OtherAllocatorType>&& other)
		{
			if (other.IsEmpty())
			{
				m_SSOData[0] = 0;
				return;
			}

			if (other.m_UsesSSO)
			{
				Memory::Copy(m_SSOData, other.m_SSOData, other.m_Size * sizeof(CharType));
				m_Size = other.m_Size;

				other.m_Size = 1;
				other.m_SSOData[0] = 0;
			}
			else
			{
				if (CompareAllocators<AllocatorType, OtherAllocatorType>(m_Allocator, other.m_Allocator))
				{
					m_Data = other.m_Data;
					m_Capacity = other.m_Capacity;
					m_Size = other.m_Size;
					m_UsesSSO = false;

					other.m_Size = 1;
					other.m_SSOData[0] = 0;
					other.m_UsesSSO = true;
				}
				else
				{
					if (FitsOnSSO(other.m_Size))
					{
						Memory::Copy(m_SSOData, other.m_Data, other.m_Size * sizeof(CharType));
					}
					else
					{
						ReAllocateNoCopy(other.m_Size);
						Memory::Copy(m_Data, other.m_Data, other.m_Size * sizeof(CharType));
					}

					m_Size = other.m_Size;
					other.Clear();
				}
			}
		}

		void CopyOperator(const BasicStringView<CharType>& other)
		{
			uint64 other_size = other.Length() + 1;
			const CharType* other_data = other.Data();

			if (other_size <= 1)
			{
				Clear();
				return;
			}

			if (m_UsesSSO)
			{
				if (FitsOnSSO(other_size))
				{
					Memory::Copy(m_SSOData, other_data, (other_size - 1) * sizeof(CharType));
					m_SSOData[other_size - 1] = 0;
					m_Size = other_size;
				}
				else
				{
					ReAllocateNoCopy(other_size);
					Memory::Copy(m_Data, other_data, (other_size - 1) * sizeof(CharType));
					m_Data[other_size - 1] = 0;
					m_Size = other_size;
				}
			}
			else
			{
				if (other_size > m_Capacity)
					ReAllocateNoCopy(other_size);

				Memory::Copy(m_Data, other_data, (other_size - 1) * sizeof(CharType));
				m_Data[other_size - 1] = 0;
				m_Size = other_size;
			}
		}

		template<typename OtherAllocatorType>
		void MoveOperator(BasicString<CharType, OtherAllocatorType>&& other)
		{
			if (other.IsEmpty())
			{
				Clear();
				return;
			}

			if (other.m_UsesSSO)
			{
				Memory::Copy(Data(), other.m_SSOData, other.m_Size * sizeof(CharType));
				m_Size = other.m_Size;

				other.m_Size = 1;
				other.m_SSOData[0] = 0;
			}
			else
			{
				if (CompareAllocators<AllocatorType, OtherAllocatorType>(m_Allocator, other.m_Allocator))
				{
					if (!m_UsesSSO)
						FreeData();

					m_Data = other.m_Data;
					m_Capacity = other.m_Capacity;
					m_Size = other.m_Size;
					m_UsesSSO = false;

					other.m_Size = 1;
					other.m_SSOData[0] = 0;
					other.m_UsesSSO = true;
				}
				else
				{
					if (m_UsesSSO)
					{
						if (FitsOnSSO(other.m_Size))
						{
							Memory::Copy(m_SSOData, other.m_Data, other.m_Size * sizeof(CharType));
						}
						else
						{
							ReAllocateNoCopy(other.m_Size);
							Memory::Copy(m_Data, other.m_Data, other.m_Size * sizeof(CharType));
						}
					}
					else
					{
						if (other.m_Size > m_Capacity)
							ReAllocateNoCopy(other.m_Size);
						Memory::Copy(m_Data, other.m_Data, other.m_Size * sizeof(CharType));
					}

					m_Size = other.m_Size;
					other.Clear();
				}
			}
		}

	private:
		static void FormatI(BasicStringView<CharType>& format, BasicString& out_result)
		{
			out_result.Append(format);
			format.m_String += format.m_Length;
			format.m_Length = 0;
		}

		template<typename T>
		static void FormatI(BasicStringView<CharType>& format, BasicString& out_result, T&& value)
		{
			for (SizeT index = 0; index < format.m_Length; index++)
			{
				if (format.m_String[index] == '%' && format.m_String[index + 1] == '{')
				{
					index += 2;
					SizeT i = index;

					bool is_valid = false;
					for (index = index; index < format.m_Length; index++)
					{
						if (format.m_String[index] == '}')
						{
							is_valid = true;
							break;
						}
					}

					if (!is_valid)
						// TODO (Avr): Report the error to the user.
						continue;

					BasicStringView<CharType> flags = BasicStringView<CharType>(format.m_String + i, index - i);

					using ArgumentType = ArrayToPointerDecayType<RemoveReferenceType<T>>;
					ToString<ArgumentType, CharType, AllocatorType>::Get(out_result, value, flags);

					format.m_String += (index + 1);
					format.m_Length -= (index + 1);
					return;
				}
				else
				{
					out_result.AppendChar(format.m_String[index]);
				}
			}

			format.m_String += format.m_Length;
			format.m_Length = 0;
		}

		template<typename T, typename... Args>
		static void FormatI(BasicStringView<CharType>& format, BasicString& out_result, T&& value, Args&&... args)
		{
			FormatI(format, out_result, Leaf::Forward<T>(value));
			FormatI(format, out_result, Leaf::Forward<Args>(args)...);
		}

	private:
		void ReAllocate(uint64 new_capacity)
		{
			CharType* new_block = (CharType*)m_Allocator.AllocateTaggedI(new_capacity * sizeof(CharType));

			if (new_capacity < m_Size)
				m_Size = new_capacity;
			new_block[m_Size - 1] = 0;

			if (m_UsesSSO)
			{
				Memory::Copy(new_block, m_SSOData, (m_Size - 1) * sizeof(CharType));
			}
			else
			{
				Memory::Copy(new_block, m_Data, (m_Size - 1) * sizeof(CharType));
				FreeData();
			}

			m_Data = new_block;
			m_Capacity = new_capacity;
			m_UsesSSO = false;
		}

		void ReAllocateNoCopy(uint64 new_capacity)
		{
			CharType* new_block = (CharType*)m_Allocator.AllocateTaggedI(new_capacity * sizeof(CharType));

			if (!m_UsesSSO)
				FreeData();

			m_Data = new_block;
			m_Capacity = new_capacity;
			m_UsesSSO = false;
		}

		void FreeData()
		{
			m_Allocator.Free(m_Data, m_Capacity * sizeof(CharType));
		}

	private:
		union
		{
			struct
			{
				CharType* m_Data;
				uint64 m_Capacity;
			};

			CharType m_SSOData[SSOSize];
		};

		uint64 m_Size;
		AllocatorType m_Allocator;
		bool m_UsesSSO;

	private:
		template<typename FriendCharType>
		friend class BasicStringView;
	};

	using String = BasicString<char, HeapAllocator>;

}