#include "TimeLimit.h"

namespace Memory
{
	TimeLimit::TimeLimit()
		: m_startTime(Clock::now())
		, m_duration(0)
		, m_current(0)
		, m_interval(0)
		, m_count(0)
	{}

	TimeLimit::TimeLimit(const Duration& limit)
		: m_startTime(Clock::now())
		, m_duration(limit)
		, m_current(0)
		, m_interval(0)
		, m_count(0)
	{}

	void TimeLimit::Init(const size_t limit)
	{
		m_duration = Duration(limit);
	}

	void TimeLimit::Start()
	{
		m_startTime = Clock::now();
		m_current = m_duration;
	}

	void TimeLimit::Update()
	{
		if (m_count < m_interval)
		{
			m_count++;
		}
		else
		{
			auto currentTime = Clock::now();
			auto timeDuration = currentTime - m_startTime;
			if (timeDuration > m_duration)
			{
				m_current = Duration(0);
			}
			else
			{
				m_current -= timeDuration;
			}

			m_startTime = currentTime;

			m_count = 0;
		}
	}

	void TimeLimit::SetInterval(const int interval)
	{
		m_interval = interval;
		m_count = 0;
	}

	bool TimeLimit::HasTime()
	{
		static Duration zero(0);

		Update();

		return m_current > zero;
	}
}