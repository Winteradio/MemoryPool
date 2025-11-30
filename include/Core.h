#ifndef __MEMORY_CORE_H__
#define __MEMORY_CORE_H__

#include "Storage/StorageManager.h"
#include "Collector/GarbageCollector.h"
#include "Pointer/ObjectPtr.h"

namespace Memory
{
	StorageManager& GetStorage();
	GarbageCollector& GetCollector();

	void Init(const size_t poolSize, const size_t sweepTime);
	void Release();
	void Collect();

	template<typename T, typename... Args>
	ObjectPtr<T> MakePtr(Args&&... args)
	{
		Accessor<T>* accessor = GetStorage().Create<T>(std::forward<Args>(args)...);

		return ObjectPtr<T>(accessor);
	}

	template<typename T, typename... Args>
	ObjectPtr<T[]> MakeArray(const size_t count, Args&&... args)
	{
		Accessor<T>* accessor = GetStorage().CreateArray<T>(count ,std::forward<Args>(args)...);

		return ObjectPtr<T[]>(accessor);
	}

	template<typename T, typename U>
	const ObjectPtr<T> Cast(const ObjectPtr<U>& other)
	{
		static_assert(!Reflection::Utils::IsConst<U>::value || Reflection::Utils::IsConst<T>::value,
			"Memory::Cast<T, U> : If the U is the const qualifier, the T must be the const qualifier.");

		U* otherInstance = (&*other);
		T* instance = Reflection::Cast<T*, U*>(otherInstance);
		if (nullptr != instance)
		{
			return ObjectPtr<T>(other.GetAccessor());
		}
		else
		{
			return ObjectPtr<T>();
		}
	}

	template<typename T, typename U>
	ObjectPtr<T> Cast(ObjectPtr<U>& other)
	{
		static_assert(!Reflection::Utils::IsConst<U>::value || Reflection::Utils::IsConst<T>::value,
			"Memory::Cast<T, U> : If the U is the const qualifier, the T must be the const qualifier.");

		U* otherInstance = (&*other);
		T* instance = Reflection::Cast<T*, U*>(otherInstance);
		if (nullptr != instance)
		{
			return ObjectPtr<T>(other.GetAccessor());
		}
		else
		{
			return ObjectPtr<T>();
		}
	}

	template<typename T, typename U, 
		typename Return = typename Reflection::Utils::Conditional<Reflection::Utils::IsConst<U>::value && !Reflection::Utils::IsConst<T>::value, const T, T>::Type>
	ObjectPtr<T> ConstCast(const ObjectPtr<U>& other)
	{
		U* otherInstance = (&*other);
		Return* instance = Reflection::Cast<Return*, U*>(otherInstance);
		if (nullptr != instance)
		{
			return ObjectPtr<T>(other.GetAccessor());
		}
		else
		{
			return ObjectPtr<T>();
		}
	}
};

#endif // __MEMORY_CORE_H__