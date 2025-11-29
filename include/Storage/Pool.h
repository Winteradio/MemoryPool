#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include "Storage/IStorage.h"
#include "Container/Arena.h"
#include "Accessor/Accessor.h"
#include "TimeLimit.h"

#include <vector>
#include <algorithm>

namespace Memory
{
	class IPool : public IStorage
	{
	public :
		IPool() = default;
		virtual ~IPool() = default;

	public :
		virtual void Scan() = 0;
	};

	template<typename T>
	class Pool : public IPool
	{
		public :
			Pool()
				: m_freeList()
				, m_usedList()
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

				m_freeList.reserve(m_totalCount);
				m_usedList.reserve(m_totalCount);
				m_scanList.reserve(m_totalCount);

				for (size_t index = 0; index < m_totalCount; index++)
				{
					uint8_t* freeChunk = static_cast<uint8_t*>(m_memory) + (index * m_chunkSize);
					m_freeList.push_back(freeChunk);
				}
			}

			void Clear()
			{
				if (nullptr == m_memory)
				{
					return;
				}

				m_usedList.insert(m_usedList.end(), m_scanList.begin(), m_scanList.end());

				while (!m_usedList.empty())
				{
					uint8_t* address = m_usedList.back();
					m_usedList.pop_back();

					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);
					if (nullptr != accessor)
					{
						accessor->Destruct();
						accessor->~IAccessor();
					}
				}

				m_freeList.clear();
				m_usedList.clear();
				m_scanList.clear();

				m_arena.Deallocate(m_memory);
			}

			IAccessor* Acquire() override
			{
				if (m_freeList.empty())
				{
					return nullptr;
				}

				uint8_t* address = m_freeList.back();
				m_usedList.push_back(address);
				m_freeList.pop_back();

				IAccessor* accessor = new (address) Accessor<T>(1);
				return accessor;
			}

			void Scan()
			{
				std::swap(m_usedList, m_scanList);
			}

			bool Sweep(TimeLimit& timeLimit)
			{
				timeLimit.SetInterval(10);

				while (!m_scanList.empty())
				{
					uint8_t* address = m_scanList.back();
					m_scanList.pop_back();

					IAccessor* accessor = reinterpret_cast<IAccessor*>(address);

					const IAccessor::eStatus status = accessor->GetStatus();
					if (IAccessor::eStatus::eMarked == status)
					{
						m_usedList.push_back(address);
					}
					else
					{
						m_freeList.push_back(address);

						accessor->Destruct();
						accessor->~IAccessor();
					}

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
				const size_t usedCount = m_usedList.size();
				const float ratio = static_cast<float>(usedCount) / static_cast<float>(m_totalCount);
				return ratio;
			}

		private :
			std::vector<uint8_t*> m_freeList;
			std::vector<uint8_t*> m_usedList;
			std::vector<uint8_t*> m_scanList;

			Arena m_arena;

			size_t m_chunkSize = 0;
			size_t m_totalCount = 0;
			
			void* m_memory;
	};
};

#endif // __MEMORY_POOL_H__