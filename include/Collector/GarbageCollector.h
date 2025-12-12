#ifndef __MEMORY_GARBAGECOLLECTOR_H__
#define __MEMORY_GARBAGECOLLECTOR_H__


#include "TimeLimit.h"

#include <Container/include/HashSet.h>
#include <Container/include/DynamicArray.h>

#include <mutex>

namespace Memory
{
	class BasePtr;

	class GarbageCollector
	{
		public :
			enum eStatus : uint8_t
			{
				eIdle = 0,
				eMarking = 1,
				eSweeping = 2,
				ePurging = 3,
			};

		public :
			GarbageCollector();
			~GarbageCollector();

		public :
			void Init(const size_t sweepLimit);
			void AddRoot(const BasePtr* rootPtr);
			void RemoveRoot(const BasePtr* rootPtr);
			void Collect();

			eStatus GetStatus() const;

		private :
			void Prepare();
			void Mark();
			void Sweep();
			void Purge();

		private :
			wtr::HashSet<const BasePtr*> m_rootSet;
			wtr::DynamicArray<const BasePtr*> m_graphStack;
			TimeLimit m_timeLimit;
			std::mutex m_mutex;
			eStatus m_status;
	};
};

#endif // __MEMORY_GARBAGECOLLECTOR_H__