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
		if (!m_poolMap.empty() || !m_arrayMap.empty())
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
		m_poolMap.clear();

		for (auto& bucketPair : m_arrayMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Release();
		}
		m_arrayMap.clear();
	}

	void StorageManager::Scan()
	{
		LOGINFO() << "[MEMORY] Start the Scan";

		for (auto& bucketPair : m_poolMap)
		{
			auto& bucket = bucketPair.second;
			bucket.Scan();
		}

		LOGINFO() << "[MEMORY] Complete the Scan";
	}

	bool StorageManager::Sweep(TimeLimit& timeLimit)
	{
		LOGINFO() << "[MEMORY] Start the Sweep";

		timeLimit.Update();

		for (auto& bucketPair : m_poolMap)
		{
			auto& bucket = bucketPair.second;
			if (!bucket.Sweep(timeLimit))
			{
				return false;
			}
		}

		for (auto& bucketPair : m_arrayMap)
		{
			auto& bucket = bucketPair.second;
			if (!bucket.Sweep(timeLimit))
			{
				return false;
			}
		}

		LOGINFO() << "[MEMORY] Complete the Sweep";

		return true;
	}
}