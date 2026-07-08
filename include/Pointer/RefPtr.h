#ifndef __MEMORY_REFPTR_H__
#define __MEMORY_REFPTR_H__

#include <Reflection/include/Utils.h>
#include <Reflection/include/Type/TypeMacro.h>
#include <assert.h>
#include <atomic>

namespace Memory
{
	class RefCounted
	{
		GENERATE(RefCounted);
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
		GENERATE(RefData);
	public :
		template<typename... Args>
		RefData(Args&&... args)
			: RefCounted()
			, m_instance(std::forward<Args>(args)...)
		{}

		virtual ~RefData() = default;

		T* GetInstance()
		{
			return &m_instance;
		}

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

		RefPtr(T* instance, RefCounted* refCounted = nullptr)
			: m_refInstance(instance)
			, m_refCounted(refCounted)
		{
			if (nullptr != refCounted)
			{
				refCounted->AddRef();
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
			Reset();

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

		bool operator==(const RefPtr<T>& other) const
		{
			return m_refInstance == other.m_refInstance && m_refCounted == other.m_refCounted;
		}

		bool operator!=(const RefPtr<T>& other) const
		{
			return !(this->operator==(other));
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		bool operator==(const RefPtr<U>& other) const
		{
			return m_refInstance == static_cast<T*>(other.m_refInstance) && m_refCounted == other.m_refCounted;
		}

		template<typename U,
			typename = Reflection::Utils::IsEnabled_t<
			Reflection::Utils::IsSame<T, U>::value || Reflection::Utils::IsBase<T, U>::value>>
		bool operator!=(const RefPtr<U>& other) const
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

		operator bool() const
		{
			return nullptr != m_refInstance;
		}

		bool operator!() const
		{
			return !(this->operator bool());
		}

		RefCounted* GetRefData() const
		{
			return m_refCounted;
		}

		T* Get() const
		{
			return m_refInstance;
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

namespace std
{
	template<typename T>
	struct hash<Memory::RefPtr<T>>
	{
		size_t operator()(const Memory::RefPtr<T>& ptr) const
		{
			return hash<T*>()(ptr.Get());
		}
	};
}

#endif // __MEMORY_REFPTR_H__