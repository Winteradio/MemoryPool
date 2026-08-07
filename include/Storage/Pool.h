#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include <Container/include/Arena.h>
#include <Container/include/DynamicArray.h>

#include "Storage/IStorage.h"
#include "Accessor/Accessor.h"
#include "TimeLimit.h"

#include <vector>
#include <algorithm>

namespace Memory
{
	template<typename T>
	class Pool : public IStorage
	{
		public :
			Pool()
				: m_freeList()
				, m_usedList()
				, m_deadList()
				, m_chunkSize()
				, m_totalCount()
				, m_memory(nullptr)
			{}

			virtual ~Pool()
			{
				Clear();
			}

		public :
			void Init(const size_t memorySize) override
			{
				const size_t accessorSize = sizeof(Accessor<T>);
				const size_t alignSize = alignof(T);
				const size_t paddingSize = (alignSize - (accessorSize % alignSize)) % alignSize;
				const size_t instanceSize = sizeof(T);

				const size_t chunkSize = accessorSize + paddingSize + instanceSize;
				if (memorySize < chunkSize)
				{
					return;
				}

				m_chunkSize = chunkSize;
				m_totalCount = memorySize / m_chunkSize;
				m_memory = m_arena.Allocate(m_totalCount * m_chunkSize);

				m_freeList.Reserve(m_totalCount);
				m_usedList.Reserve(m_totalCount);
				m_deadList.Reserve(m_totalCount);

				for (size_t index = 0; index < m_totalCount; index++)
				{
					uint8_t* freeChunk = static_cast<uint8_t*>(m_memory) + (index * m_chunkSize);
					m_freeList.PushBack(freeChunk);
				}
			}

			void Clear() override
			{
				if (nullptr == m_memory)
				{
					return;
				}

				m_deadList.Insert(m_deadList.end(), m_usedList.begin(), m_usedList.end());

				while (!m_deadList.Empty())
				{
					uint8_t* address = m_deadList.Back();
					m_deadList.PopBack();

					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);
					if (nullptr != accessor)
					{
						accessor->Destruct();
						accessor->~IAccessor();
					}
				}

				m_freeList.Clear();
				m_usedList.Clear();
				m_deadList.Clear();

				m_arena.Deallocate(m_memory);
			}

			IAccessor* Acquire() override
			{
				if (m_freeList.Empty())
				{
					return nullptr;
				}

				uint8_t* address = m_freeList.Back();
				m_usedList.PushBack(address);
				m_freeList.PopBack();

				IAccessor* accessor = new (address) Accessor<T>(1);
				return accessor;
			}

			void Prepare() override
			{
				if (m_usedList.Empty())
				{
					return;
				}

				for (auto* address : m_usedList)
				{
					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);
					if (nullptr != accessor)
					{
						accessor->SetStatus(IAccessor::eStatus::eUnreachable);
					}
				}
			}

			void Sweep() override
			{
				if (m_usedList.Empty())
				{
					return;
				}

				auto itr = m_usedList.Begin();
				while (itr != m_usedList.End())
				{
					uint8_t* address = *itr;
					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);

					const IAccessor::eStatus status = accessor->GetStatus();
					if (status == IAccessor::eStatus::eUnreachable)
					{
						m_deadList.PushBack(address);
						itr = m_usedList.Erase(itr);
					}
					else
					{
						itr++;
					}
				}
			}

			bool Purge(TimeLimit& timeLimit) override
			{
				if (m_deadList.Empty())
				{
					return true;
				}

				while (!m_deadList.Empty())
				{
					uint8_t* address = m_deadList.Back();
					m_deadList.PopBack();

					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);
					if (nullptr != accessor)
					{
						accessor->Destruct();
						accessor->~IAccessor();
					}

					m_freeList.PushBack(address);

					if (!timeLimit.HasTime())
					{
						return false;
					}
				}

				return true;
			}
			
			size_t GetChunkSize() const override
			{
				return m_chunkSize;
			}

			size_t GetTotalSize() const override
			{
				return m_chunkSize * m_totalCount;
			}

			size_t GetTotalCount() const override
			{
				return m_totalCount;
			}

			float GetUsedRatio() const override
			{
				const size_t usedCount = m_usedList.Size();
				const float ratio = static_cast<float>(usedCount) / static_cast<float>(m_totalCount);
				return ratio;
			}

			bool Empty() const override
			{
				return m_usedList.Size() == 0;
			}

		private :
			wtr::DynamicArray<uint8_t*> m_freeList;
			wtr::DynamicArray<uint8_t*> m_usedList;
			wtr::DynamicArray<uint8_t*> m_deadList;

			wtr::Arena m_arena;

			size_t m_chunkSize = 0;
			size_t m_totalCount = 0;
			
			void* m_memory;
	};
};

#endif // __MEMORY_POOL_H__