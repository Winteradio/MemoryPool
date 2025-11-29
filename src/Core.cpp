#include "Core.h"

namespace Memory
{
	StorageManager& GetStorage()
	{
		static StorageManager s_StorageManager;
		return s_StorageManager;
	}

	GarbageCollector& GetCollector()
	{
		static GarbageCollector s_garbageCollector;
		return s_garbageCollector;
	}

	void Init(const size_t poolSize, const size_t sweepTime)
	{
		GetStorage().Init(poolSize);
		GetCollector().Init(sweepTime);
	}

	void Release()
	{
		GetStorage().Release();
	}

	void Collect()
	{
		GetCollector().Collect();
	}
};