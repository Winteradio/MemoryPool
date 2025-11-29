#ifndef __MEMORY_GARBAGECOLLECTOR_H__
#define __MEMORY_GARBAGECOLLECTOR_H__

#include <unordered_set>
#include <mutex>
#include <vector>

#include "TimeLimit.h"

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
				eSweeping = 2
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
			void Scan();
			void Sweep();

		private :
			std::unordered_set<const BasePtr*> m_rootSet;
			std::vector<const BasePtr*> m_graphStack;
			TimeLimit m_timeLimit;
			std::mutex m_mutex;
			eStatus m_status;
	};
};

#endif // __MEMORY_GARBAGECOLLECTOR_H__