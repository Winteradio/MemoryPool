#ifndef __MEMORY_OBJECTPTR_H__
#define __MEMORY_OBJECTPTR_H__

#include "Pointer/BasePtr.h"
#include "Accessor/IAccessor.h"

#include "Reflection/include/Reflection.h"

namespace Memory
{
	template<typename T>
	class ObjectPtr : public BasePtr
	{
		GENERATE(ObjectPtr);

		public :
			ObjectPtr()
				: m_accessor(nullptr)
			{}

			ObjectPtr(IAccessor* accessor)
				: m_accessor(accessor)
				, m_instance(static_cast<T*>(accessor->GetPointer()))
			{}

			template<typename U, typename = Reflection::Utils::IsEnabled_t<Reflection::Utils::IsChild<T, U>::value>>
			ObjectPtr(const ObjectPtr<U>& other)
				: m_accessor(other.m_accessor)
				, m_instance(static_cast<T*>(other.m_instance))
			{}

			template<typename U, typename = Reflection::Utils::IsEnabled_t<Reflection::Utils::IsChild<T, U>::value>>
			ObjectPtr(ObjectPtr<U>&& other) noexcept
				: m_accessor(std::move(other.m_accessor))
				, m_instance(static_cast<T*>(other.m_instance))
			{}

			virtual ~ObjectPtr() = default;

			template<typename U, typename = Reflection::Utils::IsEnabled_t<Reflection::Utils::IsChild<T, U>::value>>
			ObjectPtr& operator=(const ObjectPtr<U>& other)
			{
				if (this != &other)
				{
					m_accessor = other.m_accessor;
					m_instance = static_cast<T*>(other.m_instance);
				}

				return *this;
			}

			template<typename U, typename = Reflection::Utils::IsEnabled_t<Reflection::Utils::IsChild<T, U>::value>>
			ObjectPtr& operator=(ObjectPtr<U>&& other)
			{
				m_accessor = std::move(other.m_accessor);
				m_instance = static_cast<T*>(other.m_instance);

				return *this;
			}

		public :
			T& operator*()
			{
				assert(nullptr != m_instance && "The instance is invalid.");

				return *m_instance;
			}

			T* operator->()
			{
				return m_instance;
			}

			const T& operator*() const
			{
				assert(nullptr != m_instance && "The instance is invalid.");

				return *m_instance;
			}

			const T* operator->() const
			{
				return m_instance;
			}

		public :
			const Reflection::TypeInfo* GetPureType() const override
			{
				return Reflection::TypeInfo::Get<T>();
			}

			const void* GetPointer() const
			{
				return static_cast<void*>(m_instance);
			}

			IAccessor* GetAccessor() const
			{
				return m_accessor;
			}

			void Mark() const
			{
				if (nullptr != m_accessor)
				{
					m_accessor->SetStatus(IAccessor::eStatus::eMarked);
				}
			}

			void Unreachable() const
			{
				if (nullptr != m_accessor)
				{
					m_accessor->SetStatus(IAccessor::eStatus::eUnreachable);
				}
			}

		protected :
			template<typename U>
			friend class ObjectPtr;

			IAccessor* m_accessor;
			T* m_instance;
	};

	template<typename T>
	class ObjectPtr<T[]> : public ObjectPtr<T>
	{
		GENERATE(ObjectPtr);

		public :
			ObjectPtr()
				: ObjectPtr<T>()
			{}

			ObjectPtr(Accessor<T>* accessor)
				: ObjectPtr<T>(accessor)
			{}

			ObjectPtr(const ObjectPtr<T[]>& other)
				: ObjectPtr<U>(other)
			{}

			ObjectPtr(ObjectPtr<T[]>&& other) noexcept
				: ObjectPtr<U>(std::move(other))
			{}

			virtual ~ObjectPtr() = default;

			ObjectPtr& operator=(const ObjectPtr<T[]>& other)
			{
				ObjectPtr<T>::operator=(other);

				return *this;
			}

			ObjectPtr& operator=(ObjectPtr<T[]>&& other)
			{
				ObjectPtr<T>::operator=(std::move(other));

				return *this;
			}

		public :
			T& operator[](const size_t index)
			{
				return ObjectPtr<T>::operator->[index];
			}

			const T& operator[](const size_t index) const
			{
				return ObjectPtr<T>::operator->[index];
			}

			size_t GetSize() const
			{
				if (nullptr == this->m_accessor)
				{
					return 0;
				}
				else
				{
					return this->m_accessor->GetCount();
				}
			}
	};
};

#endif // __MEMORY_OBJECTPTR_H__