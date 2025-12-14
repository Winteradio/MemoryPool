#ifndef __MEMORY_BASEPTR_H__
#define __MEMORY_BASEPTR_H__

namespace Reflection
{
	class TypeInfo;
};

namespace Memory
{
	class BasePtr
	{
		GENERATE(BasePtr);

		public :
			BasePtr() = default;
			virtual ~BasePtr() = default;

		public :
			virtual const Reflection::TypeInfo* GetPureType() const = 0;
			virtual const void* GetPointer() const = 0;
			virtual void Mark() const = 0;
			virtual void Unreachable() const = 0;
			virtual bool IsMarked() const = 0;
	};
};

#endif // __MEMORY_BASEPTR_H__