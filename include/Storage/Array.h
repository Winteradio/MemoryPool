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
	private :
		enum class eStatus : uint8_t
		{
			eNone = 0,
			eUsed = 1,
			eReleased = 2
		};

	public :
		Array()
			: m_chunkSize(0)
			, m_paddingSize(0)
			, m_totalCount(0)
			, m_arena()
			, m_memory(nullptr)
			, m_status(eStatus::eNone)
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

			if (m_status != eStatus::eNone)
			{
				IAccessor* accessor = static_cast<IAccessor*>(m_memory);

				accessor->Destruct();
				accessor->~IAccessor();

				m_arena.Deallocate(m_memory);
			}

			m_status = eStatus::eNone;
			m_memory = nullptr;
		}

		IAccessor* Acquire() override
		{
			if (nullptr == m_memory)
			{
				return nullptr;
			}

			if (m_status == eStatus::eReleased)
			{
				return nullptr;
			}
			else if (m_status == eStatus::eUsed)
			{
				return static_cast<IAccessor*>(m_memory);
			}
			else
			{
				m_status = eStatus::eUsed;

				IAccessor* accessor = new (m_memory) Accessor<T>(m_totalCount);
				return accessor;
			}
		}

		void Prepare() override
		{
			if (nullptr == m_memory || eStatus::eUsed != m_status)
			{
				return;
			}

			IAccessor* accessor = static_cast<IAccessor*>(m_memory);
			accessor->SetStatus(IAccessor::eStatus::eUnreachable);
		}

		void Sweep() override
		{
			if (nullptr == m_memory || eStatus::eUsed != m_status)
			{
				return;
			}

			IAccessor* accessor = static_cast<IAccessor*>(m_memory);
			const IAccessor::eStatus status = accessor->GetStatus();
			if (status == IAccessor::eStatus::eUnreachable)
			{
				m_status = eStatus::eReleased;
			}
		}

		bool Purge(TimeLimit& timeLimit) override
		{
			if (nullptr == m_memory || m_status != eStatus::eReleased)
			{
				return true;
			}

			if (!timeLimit.HasTime())
			{
				return false;
			}

			IAccessor* accessor = static_cast<IAccessor*>(m_memory);

			accessor->Destruct();
			accessor->~IAccessor();
			m_status = eStatus::eNone;
				
			return true;
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
			return Empty() ? 0.0f : 1.0f;
		}

		bool Empty() const override
		{
			return m_status != eStatus::eUsed;
		}

	private :
		size_t m_chunkSize = 0;
		size_t m_paddingSize = 0;
		size_t m_totalCount = 0;

		wtr::Arena m_arena;

		void* m_memory;

		eStatus m_status;
	};
};

#endif // __MEMORY_ARRAY_H__