#ifndef __MEMORY_ACCESSOR_H__
#define __MEMORY_ACCESSOR_H__

#include "Accessor/IAccessor.h"

namespace Memory
{
	template<typename T>
	class Accessor : public IAccessor
	{
		static_assert(alignof(T) <= 64, "The T alignment is too large for the optimized memory block.");
		static_assert(sizeof(T) > 0, "The T size is too small, so cannot pool an empty type.");

		public :
			Accessor() = delete;
			Accessor(const size_t count) noexcept
				: m_instance(nullptr)
				, m_count(count)
				, m_status(eStatus::eUnreachable)
			{}

			virtual ~Accessor() = default;

		public :
			eStatus GetStatus() const override
			{
				return m_status;
			}

			void SetStatus(const eStatus status) const override
			{
				m_status = status;
			}

			void Destruct() override
			{
				for (size_t count = 0; count < m_count; count++)
				{
					T* instance = m_instance + count;
					instance->~T();
				}

				m_status = eStatus::eUnreachable;
			}

			size_t GetCount() const override
			{
				return m_count;
			}

			void* GetPointer() const override
			{
				return static_cast<void*>(m_instance);
			}

			T* GetInstance() const
			{
				return m_instance;
			}

		public :
			template<typename... Args>
			void Construct(Args&&... args) noexcept
			{
				const size_t accessorSize = sizeof(Accessor<T>);
				const size_t alignSize = alignof(T);
				const size_t paddingSize = (alignSize - (accessorSize % alignSize)) % alignSize;
				
				uint8_t* accessor = reinterpret_cast<uint8_t*>(this);
				m_instance = reinterpret_cast<T*>(accessor + accessorSize + paddingSize);

				for (size_t count = 0; count < m_count; count++)
				{
					new (m_instance + count) T(std::forward<Args>(args)...);
				}
			}

		private :
			T* m_instance;
			const uint32_t m_count;
			mutable eStatus m_status;
	};
};

#endif // __MEMORY_ACCESSOR_H__