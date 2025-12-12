#include "Storage/StorageManager.h"

#include "Log/include/Log.h"
#include "TimeLimit.h"

namespace Memory
{
	StorageManager::StorageManager()
		: m_poolMap()
		, m_arrayMap()
		, m_poolSize(512)
	{}

	StorageManager::~StorageManager()
	{
		if (!m_poolMap.Empty() || !m_arrayMap.Empty())
		{
			Release();
		}
	}

	void StorageManager::Init(const size_t poolSize)
	{
		LOGINFO() << "[MEMORY] Initialize the storage manager, the pool size : " << poolSize;

		m_poolSize = poolSize;
	}

	void StorageManager::Release()
	{
		LOGINFO() << "[MEMORY] Release the storage manager";

		for (auto& bucketPair : m_poolMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Release();
		}
		m_poolMap.Clear();

		for (auto& bucketPair : m_arrayMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Release();
		}
		m_arrayMap.Clear();
	}

	void StorageManager::Sweep()
	{
		LOGINFO() << "[MEMORY] Start the Sweep";

		for (auto& bucketPair : m_poolMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Sweep();
		}

		for (auto& bucketPair : m_arrayMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Sweep();
		}
	}

	bool StorageManager::Purge(TimeLimit& timeLimit)
	{
		LOGINFO() << "[MEMORY] Start the Purge";

		timeLimit.Update();

		for (auto& bucketPair : m_poolMap)
		{
			auto& bucket = bucketPair.second;
			if (!bucket.Purge(timeLimit))
			{
				return false;
			}
			else
			{
				bucket.Update();
				bucket.Remove();
			}
		}

		for (auto& bucketPair : m_arrayMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Remove();
		}

		Clear();

		LOGINFO() << "[MEMORY] Done the Purge";

		return true;
	}

	void StorageManager::Clear()
	{
		LOGINFO() << "[MEMORY] Start to clear the budgets";

		auto poolItr = m_poolMap.Begin();
		while (poolItr != m_poolMap.End())
		{
			auto& bucket = poolItr->second;
			if (bucket.Empty())
			{
				poolItr = m_poolMap.Erase(poolItr);
			}
			else
			{
				poolItr++;
			}
		}

		auto arrayItr = m_arrayMap.Begin();
		while (arrayItr != m_arrayMap.End())
		{
			auto& bucket = arrayItr->second;
			if (bucket.Empty())
			{
				arrayItr = m_arrayMap.Erase(arrayItr);
			}
			else
			{
				arrayItr++;
			}
		}
	}
}