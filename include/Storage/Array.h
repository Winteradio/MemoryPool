#ifndef __MEMORY_ARRAY_H__
#define __MEMORY_ARRAY_H__

#include <Container/include/Arena.h>

#include "Storage/IStorage.h"
#include "Accessor/Accessor.h"
#include "TimeLimit.h"

namespace Memory
{
	template<typename T>
	class Array : public IStorage
	{
		public :
			Array()
				: m_chunkSize(0)
				, m_paddingSize(0)
				, m_totalCount(0)
				, m_arena()
				, m_memory(nullptr)
				, m_used(false)
			{}

			virtual ~Array()
			{
				Clear();
			}

		public :
			void Init(const size_t instanceCount) override
			{
				const size_t accessorSize = sizeof(Accessor<T>);
				const size_t alignSize = alignof(T);
				const size_t paddingSize = (alignSize - (accessorSize % alignSize)) % alignSize;
				const size_t instanceSize = sizeof(T);

				m_paddingSize = accessorSize + paddingSize;
				m_chunkSize = instanceSize;
				m_totalCount = instanceCount;
				m_memory = m_arena.Allocate(m_paddingSize + (m_chunkSize * m_totalCount));
			}

			void Clear()
			{
				if (nullptr == m_memory)
				{
					return;
				}

				IAccessor* accessor = static_cast<IAccessor*>(m_memory);
				if (nullptr == accessor)
				{
					return;
				}

				accessor->Destruct();
				accessor->~IAccessor();

				m_arena.Deallocate(m_memory);
			}

			IAccessor* Acquire() override
			{
				if (nullptr == m_memory)
				{
					return nullptr;
				}

				if (m_used)
				{
					return static_cast<IAccessor*>(m_memory);
				}
				else
				{
					m_used = true;

					IAccessor* accessor = new (m_memory) Accessor<T>(m_totalCount);
					return accessor;
				}
			}

			void Sweep()
			{
				IAccessor* accessor = static_cast<IAccessor*>(m_memory);
				if (nullptr == accessor)
				{
					return;
				}

				const IAccessor::eStatus status = accessor->GetStatus();
				if (status == IAccessor::eStatus::eUnreachable)
				{
					accessor->Destruct();
					accessor->~IAccessor();

					m_used = false;
				}
				else
				{
					accessor->SetStatus(IAccessor::eStatus::eUnreachable);
				}
			}

			size_t GetChunkSize() const override
			{
				return m_chunkSize;
			}

			size_t GetTotalSize() const override
			{
				return m_paddingSize + (m_chunkSize * m_totalCount);
			}

			size_t GetTotalCount() const override
			{
				return m_totalCount;
			}

			float GetUsedRatio() const override
			{
				return m_used ? 1.0f : 0.0f;
			}

			bool Empty() const override
			{
				return !m_used;
			}

		private :
			size_t m_chunkSize = 0;
			size_t m_paddingSize = 0;
			size_t m_totalCount = 0;

			wtr::Arena m_arena;

			void* m_memory;

			bool m_used;
	};
};

#endif // __MEMORY_ARRAY_H__