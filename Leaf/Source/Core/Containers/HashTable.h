// Copyright to Avram Traian. 2022 - 2022.
// File created on June 16 2022.

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Memory.h"
#include "Core/Assert.h"

#include "Hash.h"
#include "Comparator.h"
#include "Pair.h"

namespace Leaf {

	template<typename KeyType, typename ValueType>
	class HashTableIterator
	{
	public:
		using Entry = Pair<KeyType, ValueType>;

		enum EntryFlagsEnum : uint8
		{
			ENTRY_FLAG_Free = 0x00,
			ENTRY_FLAG_Occupied = 0xFF,
			ENTRY_FLAG_Deleted = 0xDD
		};
		using EntryFlag = uint8;

	public:
		HashTableIterator(const void* entry, const void* max_entry, const void* flag)
			: m_Entry((Entry*)entry)
			, m_MaxEntry((Entry*)max_entry)
			, m_Flag((EntryFlag*)flag)
		{}

	public:
		bool operator==(const HashTableIterator& other) const
		{
			return m_Entry == other.m_Entry;
		}

		bool operator!=(const HashTableIterator& other) const
		{
			return m_Entry != other.m_Entry;
		}

		HashTableIterator& operator++()
		{
			while (true)
			{
				m_Entry++;
				m_Flag++;

				if (m_Entry >= m_MaxEntry)
				{
					break;
				}
				if (*m_Flag == ENTRY_FLAG_Occupied)
				{
					break;
				}
			}

			return *this;
		}

		HashTableIterator operator++(int)
		{
			HashTableIterator temp = *this;
			++(*this);
			return temp;
		}

		Entry* operator->() const
		{
			return m_Entry;
		}

		Entry& operator*() const
		{
			return *m_Entry;
		}

	private:
		Entry* m_Entry;
		Entry* m_MaxEntry;
		EntryFlag* m_Flag;
	};

	template<
		typename KeyType, typename ValueType,
		typename AllocatorType = HeapAllocator,
		typename HasherType = Hasher<RemoveReferenceType<RemoveConstType<KeyType>>>,
		typename ComparatorType = Comparator<RemoveReferenceType<RemoveConstType<KeyType>>>
	>
	class HashTable
	{
	public:
		using Entry = Pair<KeyType, ValueType>;

		enum EntryFlagsEnum : uint8
		{
			ENTRY_FLAG_Free = 0x00,
			ENTRY_FLAG_Occupied = 0xFF,
			ENTRY_FLAG_Deleted = 0xDD
		};
		using EntryFlag = uint8;

		using Iterator = HashTableIterator<const KeyType, ValueType>;
		using ConstIterator = HashTableIterator<const KeyType, const ValueType>;

		static constexpr double MAX_LOAD_FACTOR = 0.75;
		static constexpr double INVERSE_MAX_LOAD_FACTOR = 1.00 / MAX_LOAD_FACTOR;

	public:
		HashTable()
			: m_Entries(nullptr)
			, m_Flags(nullptr)
			, m_Capacity(0)
			, m_Size(0)
		{}

		HashTable(const HashTable& other)
			: m_Entries(nullptr)
			, m_Flags(nullptr)
			, m_Capacity(0)
			, m_Size(0)
		{
			CopyConstructor<AllocatorType>(other);
		}

		template<typename OtherAllocatorType>
		HashTable(const HashTable<KeyType, ValueType, OtherAllocatorType>& other)
			: m_Entries(nullptr)
			, m_Flags(nullptr)
			, m_Capacity(0)
			, m_Size(0)
		{
			CopyConstructor<OtherAllocatorType>(other);
		}

		HashTable(HashTable&& other) noexcept
			: m_Entries(nullptr)
			, m_Flags(nullptr)
			, m_Capacity(0)
			, m_Size(0)
		{
			MoveConstructor<AllocatorType>(Leaf::Move(other));
		}

		template<typename OtherAllocatorType>
		HashTable(HashTable<KeyType, ValueType, OtherAllocatorType>&& other) noexcept
			: m_Entries(nullptr)
			, m_Flags(nullptr)
			, m_Capacity(0)
			, m_Size(0)
		{
			MoveConstructor<OtherAllocatorType>(Leaf::Move(other));
		}

		~HashTable()
		{
			Clear();
			FreeData();
		}

	public:
		HashTable& operator=(const HashTable& other)
		{
			CopyAssignOperator<AllocatorType>(other);
			return *this;
		}

		template<typename OtherAllocatorType>
		HashTable& operator=(const HashTable<KeyType, ValueType, OtherAllocatorType>& other)
		{
			CopyAssignOperator<OtherAllocatorType>(other);
			return *this;
		}

		HashTable& operator=(HashTable&& other) noexcept
		{
			MoveAssignOperator<AllocatorType>(Leaf::Move(other));
			return *this;
		}

		template<typename OtherAllocatorType>
		HashTable& operator=(HashTable<KeyType, ValueType, OtherAllocatorType>&& other) noexcept
		{
			MoveAssignOperator<OtherAllocatorType>(Leaf::Move(other));
			return *this;
		}

		ValueType& operator[](const KeyType& key)
		{
			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindKeyOrFree(key);

			if (m_Flags[index] == ENTRY_FLAG_Occupied)
				return m_Entries[index].B;

			new (&m_Entries[index].A) KeyType(key);
			new (&m_Entries[index].B) ValueType();
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

		ValueType& operator[](KeyType&& key)
		{
			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindKeyOrFree(key);

			if (m_Flags[index] == ENTRY_FLAG_Occupied)
				return m_Entries[index].B;

			new (&m_Entries[index].A) KeyType(Leaf::Move(key));
			new (&m_Entries[index].B) ValueType();
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

	public:
		Entry* Entries() const { return m_Entries; }

		EntryFlag* Flags() const { return m_Flags; }

		SizeT Capacity() const { return m_Capacity; }

		SizeT Size() const { return m_Size; }

	public:
		SizeT Find(const KeyType& key) const
		{
			SizeT index = HasherType::Get(key) % m_Capacity;

			while (true)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied && ComparatorType::Compare(m_Entries[index].A, key))
					return index;

				if (m_Flags[index] == ENTRY_FLAG_Free)
					return LF_INVALID_SIZE;

				index = (index + 1) % m_Capacity;
			}
		}

		SizeT FindExisting(const KeyType& key) const
		{
			SizeT index = HasherType::Get(key) % m_Capacity;

			while (true)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied && ComparatorType::Compare(m_Entries[index].A, key))
					return index;

				// The key doesn't exist.
				LF_ASSERT_RETURN(m_Flags[index] != ENTRY_FLAG_Free, LF_INVALID_SIZE);

				index = (index + 1) % m_Capacity;
			}
		}

		bool Contains(const KeyType& key) const
		{
			return (Find(key) != LF_INVALID_SIZE);
		}

		ValueType& At(const KeyType& key)
		{
			return m_Entries[FindExisting(key)].B;
		}

		const ValueType& At(const KeyType& key) const
		{
			return m_Entries[FindExisting(key)].B;
		}

		ValueType& Add(const KeyType& key, const ValueType& value)
		{
			// The key already exists.
			LF_ASSERT_RETURN(Contains(key) == false, m_Entries[0].B);

			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindFreeIndex(key);

			new (&m_Entries[index].A) KeyType(key);
			new (&m_Entries[index].B) ValueType(value);
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

		ValueType& Add(const KeyType& key, ValueType&& value)
		{
			// The key already exists.
			LF_ASSERT_RETURN(Contains(key) == false, m_Entries[0].B);

			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindFreeIndex(key);

			new (&m_Entries[index].A) KeyType(key);
			new (&m_Entries[index].B) ValueType(Leaf::Move(value));
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

		ValueType& Add(KeyType&& key, const ValueType& value)
		{
			// The key already exists.
			LF_ASSERT_RETURN(Contains(key) == false, m_Entries[0].B);

			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindFreeIndex(key);

			new (&m_Entries[index].A) KeyType(Leaf::Move(key));
			new (&m_Entries[index].B) ValueType(value);
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

		ValueType& Add(KeyType&& key, ValueType&& value)
		{
			// The key already exists.
			LF_ASSERT_RETURN(Contains(key) == false, m_Entries[0].B);

			if (IsOverloaded())
				ReAllocate(NextCapacity(m_Size + 1));

			SizeT index = FindFreeIndex(key);

			new (&m_Entries[index].A) KeyType(Leaf::Move(key));
			new (&m_Entries[index].B) ValueType(Leaf::Move(value));
			m_Flags[index] = ENTRY_FLAG_Occupied;

			m_Size++;
			return m_Entries[index].B;
		}

		void Remove(const KeyType& key)
		{
			SizeT index = FindExisting(key);

			m_Entries[index].~Entry();
			m_Flags[index] = ENTRY_FLAG_Deleted;
			m_Size--;
		}

		void RemoveIndex(SizeT index)
		{
			// The index is out of range.
			LF_ASSERT_RETURN_VOID(index < m_Capacity);

			// The index isn't a valid entry.
			LF_ASSERT_RETURN_VOID(m_Flags[index] == ENTRY_FLAG_Occupied);

			m_Entries[index].~Entry();
			m_Flags[index] = ENTRY_FLAG_Deleted;
			m_Size--;
		}

		void RemoveIfExists(const KeyType& key)
		{
			SizeT index = Find(key);
			if (index == LF_INVALID_SIZE)
				return;

			m_Entries[index].~Entry();
			m_Flags[index] = ENTRY_FLAG_Deleted;
			m_Size--;
		}

		void RemoveIndexIfExists(SizeT index)
		{
			if (m_Flags[index] != ENTRY_FLAG_Occupied)
				return;

			m_Entries[index].~Entry();
			m_Flags[index] = ENTRY_FLAG_Deleted;
			m_Size--;
		}

	public:
		void Clear()
		{
			if (m_Size == 0)
				return;

			for (SizeT index = 0; index < m_Capacity; index++)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied)
					m_Entries[index].~Entry();
			}

			Memory::Set(m_Flags, ENTRY_FLAG_Free, m_Capacity * sizeof(EntryFlag));
			m_Size = 0;
		}

	private:
		void ReAllocate(SizeT new_capacity)
		{
			void* new_block = m_AllocatorInstance.AllocateTagged(new_capacity * (sizeof(Entry) + sizeof(EntryFlag)));
			Entry* new_entries = (Entry*)new_block;
			EntryFlag* new_flags = (EntryFlag*)(new_entries + new_capacity);

			Memory::Set(new_flags, ENTRY_FLAG_Free, new_capacity * sizeof(EntryFlag));

			if (m_Size > 0)
			{
				for (SizeT i = 0; i < m_Capacity; i++)
				{
					if (m_Flags[i] == ENTRY_FLAG_Occupied)
					{
						SizeT index = HasherType::Get(m_Entries[i].A) % new_capacity;

						while (true)
						{
							if (new_flags[index] != ENTRY_FLAG_Occupied)
							{
								new (new_entries + index) Entry(Leaf::Move(m_Entries[i]));
								new_flags[index] = ENTRY_FLAG_Occupied;
								break;
							}

							index = (index + 1) % new_capacity;
						}

						m_Entries[i].~Entry();
					}
				}
			}

			FreeData();

			m_Entries = new_entries;
			m_Flags = new_flags;
			m_Capacity = new_capacity;
		}

		void FreeData()
		{
			m_AllocatorInstance.Free(m_Entries, m_Capacity * (sizeof(Entry) + sizeof(EntryFlag)));

			m_Entries = nullptr;
			m_Flags = nullptr;
			m_Capacity = 0;
		}

		LF_INLINE SizeT NextCapacity(SizeT required_new_size) const
		{
			SizeT default_new_cap = 2 * m_Capacity + 1;
			SizeT required_new_cap = (SizeT)(required_new_size * INVERSE_MAX_LOAD_FACTOR) + 1;

			return default_new_cap > required_new_cap ? default_new_cap : required_new_cap;
		}

		LF_INLINE bool IsOverloaded() const
		{
			return m_Size >= m_Capacity * MAX_LOAD_FACTOR;
		}

	private:
		template<typename OtherAllocatorType>
		void CopyConstructor(const HashTable<KeyType, ValueType, OtherAllocatorType>& other)
		{
			if (other.m_Size == 0)
				return;

			ReAllocate(other.m_Size * INVERSE_MAX_LOAD_FACTOR + 1);

			for (SizeT index = 0; index < other.m_Capacity; index++)
			{
				if (other.m_Flags[index] == ENTRY_FLAG_Occupied)
				{
					auto& [key, value] = other.m_Entries[index];
					Add(key, value);
				}
			}
		}

		template<typename OtherAllocatorType>
		void MoveConstructor(HashTable<KeyType, ValueType, OtherAllocatorType>&& other)
		{
			if (CompareAllocators(m_AllocatorInstance, other.m_AllocatorInstance))
			{
				m_Entries = other.m_Entries;
				m_Flags = other.m_Flags;
				m_Capacity = other.m_Capacity;
				m_Size = other.m_Size;

				other.m_Entries = nullptr;
				other.m_Flags = nullptr;
				other.m_Capacity = 0;
			}
			else
			{
				if (other.m_Size > 0)
				{
					ReAllocate((SizeT)(other.m_Size * INVERSE_MAX_LOAD_FACTOR) + 1);

					for (SizeT index = 0; index < other.m_Capacity; index++)
					{
						if (other.m_Flags[index] == ENTRY_FLAG_Occupied)
						{
							auto&& [key, value] = other.m_Entries[index];
							Add(Leaf::Move(key), Leaf::Move(value));
							other.m_Entries[index].~Entry();
						}
					}
				}

				Memory::Set(other.m_Flags, ENTRY_FLAG_Free, other.m_Capacity * sizeof(EntryFlag));
			}

			other.m_Size = 0;
		}

		template<typename OtherAllocatorType>
		void CopyAssignOperator(const HashTable<KeyType, ValueType, OtherAllocatorType>& other)
		{
			Clear();

			if (other.m_Size == 0)
				return;

			if (other.m_Size > m_Capacity * MAX_LOAD_FACTOR)
				ReAllocate(NextCapacity(other.m_Size));

			for (SizeT index = 0; index < other.m_Capacity; index++)
			{
				if (other.m_Flags[index] == ENTRY_FLAG_Occupied)
				{
					auto& [key, value] = other.m_Entries[index];
					Add(key, value);
				}
			}
		}

		template<typename OtherAllocatorType>
		void MoveAssignOperator(HashTable<KeyType, ValueType, OtherAllocatorType>&& other)
		{
			Clear();

			if (CompareAllocators(m_AllocatorInstance, other.m_AllocatorInstance))
			{
				Entry* temp_entries = m_Entries;
				EntryFlag* temp_flags = m_Flags;
				SizeT temp_capacity = m_Capacity;

				m_Entries = other.m_Entries;
				m_Flags = other.m_Flags;
				m_Capacity = other.m_Capacity;
				m_Size = other.m_Size;

				other.m_Entries = temp_entries;
				other.m_Flags = temp_flags;
				other.m_Capacity = temp_capacity;
			}
			else
			{
				if (other.m_Size > 0)
				{
					if (other.m_Size > m_Capacity * MAX_LOAD_FACTOR)
						ReAllocate(NextCapacity(other.m_Size));

					for (SizeT index = 0; index < other.m_Capacity; index++)
					{
						if (other.m_Flags[index] == ENTRY_FLAG_Occupied)
						{
							auto&& [key, value] = other.m_Entries[index];
							Add(Leaf::Move(key), Leaf::Move(value));
							other.m_Entries[index].~Entry();
						}
					}
				}
			}

			Memory::Set(other.m_Flags, ENTRY_FLAG_Free, other.m_Capacity * sizeof(EntryFlag));
			other.m_Size = 0;
		}

	private:
		SizeT FindFreeIndex(const KeyType& key) const
		{
			SizeT index = HasherType::Get(key) % m_Capacity;

			while (true)
			{
				if (m_Flags[index] != ENTRY_FLAG_Occupied)
					return index;

				index = (index + 1) % m_Capacity;
			}
		}

		SizeT FindKeyOrFree(const KeyType& key) const
		{
			SizeT index = HasherType::Get(key) % m_Capacity;
			SizeT free_index = LF_INVALID_SIZE;

			while (true)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied && ComparatorType::Compare(m_Entries[index].A, key))
					return index;

				if (m_Flags[index] == ENTRY_FLAG_Free)
				{
					if (free_index == LF_INVALID_SIZE)
						free_index = index;

					return free_index;
				}

				if (m_Flags[index] == ENTRY_FLAG_Deleted && free_index == LF_INVALID_SIZE)
					free_index = index;

				index = (index + 1) % m_Capacity;
			}
		}

	public:
		Iterator CreateIterator(SizeT index)
		{
			return Iterator(m_Entries + index, m_Entries + m_Capacity, m_Flags + index);
		}

		ConstIterator CreateIterator(SizeT index) const
		{
			return ConstIterator(m_Entries + index, m_Entries + m_Capacity, m_Flags + index);
		}

		Iterator begin()
		{
			for (SizeT index = 0; index < m_Capacity; index++)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied)
					return CreateIterator(index);
			}

			return end();
		}

		Iterator end()
		{
			return CreateIterator(m_Capacity);
		}

		ConstIterator begin() const
		{
			for (SizeT index = 0; index < m_Capacity; index++)
			{
				if (m_Flags[index] == ENTRY_FLAG_Occupied)
					return CreateIterator(index);
			}

			return end();
		}

		ConstIterator end() const
		{
			return CreateIterator(m_Capacity);
		}

	private:
		Entry* m_Entries;
		SizeT m_Capacity;
		SizeT m_Size;
		AllocatorType m_AllocatorInstance;
		EntryFlag* m_Flags;
	};

}