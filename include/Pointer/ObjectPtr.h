#ifndef __MEMORY_OBJECTPTR_H__
#define __MEMORY_OBJECTPTR_H__

#include "Pointer/BasePtr.h"
#include "Accessor/IAccessor.h"

#include "Reflection/include/Reflection.h"

#include <cassert>

namespace Memory
{
	template<typename T>
	class ObjectPtr : public BasePtr
	{
		GENERATE(ObjectPtr);

		public :
			ObjectPtr()
				: m_accessor(nullptr)
				, m_instance(nullptr)
			{}

			ObjectPtr(const IAccessor* accessor)
				: m_accessor(accessor)
				, m_instance(static_cast<T*>(accessor->GetPointer()))
			{}

			ObjectPtr(const ObjectPtr& other)
				: m_accessor(other.m_accessor)
				, m_instance(static_cast<T*>(other.m_instance))
			{}

			ObjectPtr(ObjectPtr&& other) noexcept
				: m_accessor(std::move(other.m_accessor))
				, m_instance(static_cast<T*>(other.m_instance))
			{}

			virtual ~ObjectPtr() = default;

			ObjectPtr& operator=(const ObjectPtr& other)
			{
				if (this != &other)
				{
					m_accessor = other.m_accessor;
					m_instance = static_cast<T*>(other.m_instance);
				}

				return *this;
			}

			ObjectPtr& operator=(ObjectPtr&& other)
			{
				if (this != &other)
				{
					m_accessor = std::move(other.m_accessor);
					m_instance = static_cast<T*>(other.m_instance);
				}

				return *this;
			}

			template<typename U,
				typename = Reflection::Utils::IsEnabled_t<
				Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>
			>
			ObjectPtr(const ObjectPtr<U>& other)
				: m_accessor(other.m_accessor)
				, m_instance(static_cast<T*>(other.m_instance))
			{
			}

			template<typename U,
				typename = Reflection::Utils::IsEnabled_t<
				Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>
			>
			ObjectPtr(ObjectPtr<U>&& other) noexcept
				: m_accessor(std::move(other.m_accessor))
				, m_instance(static_cast<T*>(other.m_instance))
			{
			}

			template<typename U, 
				typename = Reflection::Utils::IsEnabled_t<
					Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>
			>
			ObjectPtr& operator=(const ObjectPtr<U>& other)
			{
				m_accessor = other.m_accessor;
				m_instance = static_cast<T*>(other.m_instance);

				return *this;
			}

			template<typename U, 
				typename = Reflection::Utils::IsEnabled_t<
					Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>
			>
			ObjectPtr& operator=(ObjectPtr<U>&& other)
			{
				m_accessor = std::move(other.m_accessor);
				m_instance = static_cast<T*>(other.m_instance);

				return *this;
			}

		public :
			T& operator*()
			{
				assert(nullptr != m_accessor && "The accessor is invalid.");

				return *m_instance;
			}

			T* operator->()
			{
				return m_instance;
			}

			T& operator*() const
			{
				assert(nullptr != m_accessor && "The accessor is invalid.");

				return *m_instance;
			}

			T* operator->() const
			{
				return m_instance;
			}

			explicit operator bool() const
			{
				return IsValid();
			}

			bool operator!() const
			{
				return !IsValid();
			}

			bool operator==(const ObjectPtr<T>& other)
			{
				return m_instance == other.m_instance;
			}

			template<typename U,
				typename = Reflection::Utils::IsEnabled_t<
				Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>
			>
			bool operator==(const ObjectPtr<U>& other)
			{
				return m_instance == other.m_instance;
			}

		public :
			const Reflection::TypeInfo* GetPureType() const override
			{
				return Reflection::TypeInfo::Get<T>();
			}

			const Reflection::TypeInfo* GetRuntimeType() const override
			{
				if (IsValid())
				{
					if constexpr (Reflection::Utils::HasRuntimeType<T>::value)
					{
						return m_instance->GetTypeInfo();
					}
				}

				return GetPureType();
			}

			const void* GetPointer() const
			{
				return static_cast<const void*>(m_instance);
			}

			const IAccessor* GetAccessor() const
			{
				return m_accessor;
			}

			void Mark() const override
			{
				if (nullptr != m_accessor)
				{
					m_accessor->SetStatus(IAccessor::eStatus::eMarked);
				}
			}

			void Unreachable() const override
			{
				if (nullptr != m_accessor)
				{
					m_accessor->SetStatus(IAccessor::eStatus::eUnreachable);
				}
			}

			bool IsMarked() const override
			{
				if (nullptr != m_accessor)
				{
					return IAccessor::eStatus::eMarked == m_accessor->GetStatus();
				}
				else
				{
					return false;
				}
			}

			bool IsValid() const override
			{
				return nullptr != m_accessor && nullptr != m_instance;
			}

			void Reset()
			{
				m_accessor = nullptr;
				m_instance = nullptr;
			}

		protected :
			template<typename U>
			friend class ObjectPtr;

			const IAccessor* m_accessor;
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

			ObjectPtr(IAccessor* accessor)
				: ObjectPtr<T>(accessor)
			{}

			ObjectPtr(const ObjectPtr<T[]>& other)
				: ObjectPtr<T>(other)
			{}

			ObjectPtr(ObjectPtr<T[]>&& other) noexcept
				: ObjectPtr<T>(std::move(other))
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
				assert(index < GetSize() && "The array index out of bounds.");

				return this->m_instance[index];
			}

			T& operator[](const size_t index) const
			{
				assert(index < GetSize() && "The array index out of bounds.");

				return this->m_instance[index];
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