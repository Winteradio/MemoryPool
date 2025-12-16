#ifndef __MEMORY_TIMELIMIT_H__
#define __MEMORY_TIMELIMIT_H__

#include <chrono>

namespace Memory
{
	class TimeLimit
	{
		public :
			using Clock = std::chrono::high_resolution_clock;
			using Duration = std::chrono::milliseconds;

		public :
			TimeLimit();
			explicit TimeLimit(const Duration& limit);

		public :
			void Init(const size_t limit);
			void Start();
			void Update();
			void SetInterval(const int interval);
			bool HasTime();

		private :
			Clock::time_point m_startTime;
			Duration m_duration;
			Duration m_current;
			int m_interval;
			int m_count;
	};
};

#endif // __MEMORY_TIMELIMIT_H__