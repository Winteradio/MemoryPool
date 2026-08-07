#ifndef __MEMORY_ISTORAGE_H__
#define __MEMORY_ISTORAGE_H__

namespace Memory
{
	class IAccessor;
	class TimeLimit;

	class IStorage
	{
		public :
			virtual ~IStorage() = default;

		public :
			virtual void Init(const size_t count) = 0;
			virtual void Clear() = 0;
			
			virtual IAccessor* Acquire() = 0;
			virtual void Prepare() = 0;
			virtual void Sweep() = 0;
			virtual bool Purge(TimeLimit& timeLimit) = 0;

			virtual size_t GetChunkSize() const = 0;
			virtual size_t GetTotalSize() const = 0;
			virtual size_t GetTotalCount() const = 0;
			virtual float GetUsedRatio() const = 0; 
			virtual bool Empty() const = 0;
	};
}

#endif // __MEMORY_ISTORAGE_H__