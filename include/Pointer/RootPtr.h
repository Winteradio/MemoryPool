#ifndef __MEMORY_ROOTPTR_H__
#define __MEMORY_ROOTPTR_H__

#include "Pointer/ObjectPtr.h"
#include "Core.h"

namespace Memory
{
	template<typename T>
	class RootPtr : public ObjectPtr<T>
	{
		GENERATE(RootPtr);

		public :
			RootPtr() = delete;
			RootPtr(const ObjectPtr<T>& other)
				: ObjectPtr<T>(other)
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(ObjectPtr<T>&& other)
				: ObjectPtr<T>(std::move(other))
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(const RootPtr<T>& other)
				: ObjectPtr<T>(*other)
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(RootPtr<T>&& other)
				: ObjectPtr<T>(*other)
			{
				GetCollector().AddRoot(this);
			}

			virtual ~RootPtr()
			{
				GetCollector().RemoveRoot(this);
			}
	};

	template<typename T>
	class RootPtr<T[]> : public ObjectPtr<T[]>
	{
		GENERATE(RootPtr);

		public :
			RootPtr() = delete;
			RootPtr(const ObjectPtr<T[]>& other)
				: ObjectPtr<T[]>(other)
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(ObjectPtr<T[]>&& other)
				: ObjectPtr<T[]>(std::move(other))
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(const RootPtr<T[]>& other)
				: ObjectPtr<T[]>(*other)
			{
				GetCollector().AddRoot(this);
			}

			RootPtr(RootPtr<T[]>&& other)
				: ObjectPtr<T[]>(*other)
			{
				GetCollector().AddRoot(this);
			}

			virtual ~RootPtr()
			{
				GetCollector().RemoveRoot(this);
			}
	};
};

#endif // __MEMORY_ROOTPTR_H__