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
	class RefPtr;

	template<typename T>
	class RefData : public RefCounted
	{
	public :
		template<typename... Args>
		RefData(Args&&... args)
			: RefCounted()
			, m_instance(std::forward<Args>(args)...)
		{}

	private :
		template<typename U>
		friend class RefPtr;

		T m_instance;
	};

	template<typename T>
	class RefPtr
	{
	public:
		RefPtr()
			: m_refInstance(nullptr)
			, m_refCounted(nullptr)
		{}

		RefPtr(std::nullptr_t)
			: m_refInstance(nullptr)
			, m_refCounted(nullptr)
		{}

		template<typename U>
		RefPtr(RefData<U>* refData)
			: m_refInstance(nullptr)
			, m_refCounted(refData)
		{
			if (nullptr != refData)
			{
				m_refInstance = static_cast<T*>(&refData->m_instance);
				refData->AddRef();
			}
		}

		RefPtr(const RefPtr& other)
			: m_refInstance(other.m_refInstance)
			, m_refCounted(other.m_refCounted)
		{
			if (nullptr != m_refCounted)
			{
				m_refCounted->AddRef();
			}
		}

		RefPtr(RefPtr&& other) noexcept
			: m_refInstance(other.m_refInstance)
			, m_refCounted(other.m_refCounted)
		{
			other.m_refInstance = nullptr;
			other.m_refCounted = nullptr;
		}

		~RefPtr()
		{
			Reset();
		}

		RefPtr& operator=(std::nullptr_t)
		{
			Reset();
			
			return *this;
		}

		RefPtr& operator=(const RefPtr& other)
		{
			Reset();

			m_refInstance = other.m_refInstance;
			m_refCounted = other.m_refCounted;
			if (nullptr != m_refCounted)
			{
				m_refCounted->AddRef();
			}

			return *this;
		}

		RefPtr& operator=(RefPtr&& other) noexcept
		{
			Reset();

			m_refInstance = other.m_refInstance;
			m_refCounted = other.m_refCounted;
			other.m_refInstance = nullptr;
			other.m_refCounted = nullptr;

			return *this;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr(const RefPtr<U>& other)
			: m_refInstance(static_cast<T*>(other.m_refInstance))
			, m_refCounted(other.m_refCounted)
		{
			if (nullptr != m_refCounted)
			{
				m_refCounted->AddRef();
			}
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr(RefPtr<U>&& other) noexcept
			: m_refInstance(static_cast<T*>(other.m_refInstance))
			, m_refCounted(other.m_refCounted)
		{
			other.m_refInstance = nullptr;
			other.m_refCounted = nullptr;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr& operator=(const RefPtr<U>& other)
		{
			m_refInstance = static_cast<T*>(other.m_refInstance);
			m_refCounted = other.m_refCounted;

			if (nullptr != m_refCounted)
			{
				m_refCounted->AddRef();
			}

			return *this;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		RefPtr& operator=(RefPtr<U>&& other) noexcept
		{
			m_refInstance = static_cast<T*>(other.m_refInstance);
			m_refCounted = other.m_refCounted;
			other.m_refInstance = nullptr;
			other.m_refCounted = nullptr;

			return *this;
		}

		bool operator==(const RefPtr& other)
		{
			return m_refInstance == other.m_refInstance && m_refCounted == other.m_refCounted;
		}

		bool operator!=(const RefPtr& other)
		{
			return !(this->operator==(other));
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		bool operator==(const RefPtr<U>& other)
		{
			return m_refInstance == static_cast<T*>(other.m_refInstance) && m_refCounted == other.m_refCounted;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		bool operator!=(const RefPtr<U>& other)
		{
			return !(this->operator==(other));
		}

	public :
		T* operator->() const
		{
			assert(nullptr != m_refInstance && "The reference data is invalid");

			return m_refInstance;
		}

		T& operator*() const
		{
			assert(nullptr != m_refInstance && "The reference data is invalid");

			return *m_refInstance;
		}

		explicit operator bool() const
		{
			return nullptr != m_refInstance;
		}

		bool operator!() const
		{
			return !(this->operator bool());
		}

		void Reset()
		{
			if (nullptr != m_refCounted)
			{
				m_refCounted->Release();
				m_refCounted = nullptr;
			}

			m_refInstance = nullptr;
		}

	private:
		template<typename U>
		friend class RefPtr;

		T* m_refInstance;
		RefCounted* m_refCounted;
	};
};

#endif // __MEMORY_REFPTR_H__