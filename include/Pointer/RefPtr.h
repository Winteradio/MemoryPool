#ifndef __MEMORY_REFPTR_H__
#define __MEMORY_REFPTR_H__

#include <Reflection/include/Utils.h>
#include <assert.h>
#include <atomic>

namespace Memory
{
	class RefCounted
	{
	public:
		RefCounted()
			: m_refCount(0)
		{}

		virtual ~RefCounted()
		{}

		void AddRef()
		{
			m_refCount.fetch_add(1, std::memory_order_relaxed);
		}

		void Release()
		{
			if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				delete this;
			}
		}

		uint32_t GetRefCount() const
		{
			return m_refCount.load(std::memory_order_relaxed);
		}

	private:
		std::atomic<uint32_t> m_refCount;
	};

	template<typename T>
	class RefPtr
	{
	public:
		RefPtr()
			: m_refInstance(nullptr)
		{
			static_assert(Reflection::Utils::IsBase<RefCounted, T>::value && "The invalid type for the reference pointer");
		}

		RefPtr(T* refInstance)
			: m_refInstance(refInstance)
		{
			static_assert(Reflection::Utils::IsBase<RefCounted, T>::value && "The invalid type for the reference pointer");

			if (nullptr != m_refInstance)
			{
				m_refInstance->AddRef();
			}
		}

		RefPtr(const RefPtr& other)
			: m_refInstance(other.m_refInstance)
		{
			static_assert(Reflection::Utils::IsBase<RefCounted, T>::value && "The invalid type for the reference pointer");

			if (nullptr != m_refInstance)
			{
				m_refInstance->AddRef();
			}
		}

		RefPtr(RefPtr&& other) noexcept
			: m_refInstance(other.m_refInstance)
		{
			static_assert(Reflection::Utils::IsBase<RefCounted, T>::value && "The invalid type for the reference pointer");
			
			other.m_refInstance = nullptr;
		}

		~RefPtr()
		{
			Reset();
		}

		RefPtr& operator=(const RefPtr& other)
		{
			if (this != &other)
			{
				Reset();

				m_refInstance = other.m_refInstance;
				if (nullptr != m_refInstance)
				{
					m_refInstance->AddRef();
				}
			}

			return *this;
		}

		RefPtr& operator=(RefPtr&& other) noexcept
		{
			if (this != &other)
			{
				Reset();

				m_refInstance = other.m_refInstance;
				other.m_refInstance = nullptr;
			}

			return *this;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr(const RefPtr<U>& other)
			: m_refInstance(other.m_refInstance)
		{
			if (nullptr != m_refInstance)
			{
				m_refInstance->AddRef();
			}
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr(RefPtr<U>&& other) noexcept
			: m_refInstance(other.m_refInstance)
		{
			other.m_refInstance = nullptr;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr& operator=(const RefPtr<U>& other)
		{
			if (this != &other)
			{
				m_refInstance = other.m_refInstance;
				if (nullptr != m_refInstance)
				{
					m_refInstance->AddRef();
				}
			}

			return *this;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr& operator=(RefPtr<U>&& other) noexcept
		{
			if (this != &other)
			{
				m_refInstance = other.m_refInstance;
				other.m_refInstance = nullptr;
			}

			return *this;
		}

	public :
		T* operator->() const
		{
			return m_refInstance;
		}

		T& operator*() const
		{
			assert(nullptr != m_refInstance && "The reference instance is invalid");

			return *m_refInstance;
		}

		explicit operator bool() const
		{
			return nullptr != m_refInstance;
		}

		bool operator!() const
		{
			return nullptr == m_refInstance;
		}

		void Reset()
		{
			if (nullptr != m_refInstance)
			{
				m_refInstance->Release();
				m_refInstance = nullptr;
			}
		}

	private:
		T* m_refInstance;
	};
};

#endif // __MEMORY_REFPTR_H__