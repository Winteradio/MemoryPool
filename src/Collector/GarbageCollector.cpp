#include "Collector/GarbageCollector.h"

#include "Core.h"
#include "Log/include/Log.h"

#include <queue>

namespace Memory
{
	GarbageCollector::GarbageCollector()
		: m_rootSet()
		, m_graphStack()
		, m_timeLimit()
		, m_mutex()
		, m_status(eStatus::eIdle)
	{}

	GarbageCollector::~GarbageCollector()
	{}

	void GarbageCollector::Init(const size_t sweepTime)
	{
		m_graphStack.reserve(4096);

		m_timeLimit.Init(sweepTime);
	}

	void GarbageCollector::AddRoot(const BasePtr* rootPtr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_rootSet.insert(rootPtr);
	}

	void GarbageCollector::RemoveRoot(const BasePtr* rootPtr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_rootSet.erase(rootPtr);
	}

	void GarbageCollector::Collect()
	{
		LOGINFO() << "[COLLECTOR] Start the garbage collector";

		m_timeLimit.Start();

		if (eStatus::eIdle == m_status)
		{
			Prepare();
		}

		if (eStatus::eMarking == m_status)
		{
			Mark();
			Scan();
		}

		if (eStatus::eSweeping == m_status)
		{
			Sweep();
		}
	}

	void GarbageCollector::Prepare()
	{
		LOGINFO() << "[COLLECTOR] GC Cycle - Prepare";

		std::lock_guard<std::mutex> lock(m_mutex);

		for (const BasePtr* basePtr : m_rootSet)
		{
			m_graphStack.push_back(basePtr);
		}

		m_status = eStatus::eMarking;
	}

	void GarbageCollector::Mark()
	{
		static const Reflection::TypeInfo* baseType = Reflection::TypeInfo::Get<BasePtr>();

		LOGINFO() << "[COLLECTOR] GC Cycle - Mark";

		m_timeLimit.SetInterval(100);

		while (!m_graphStack.empty())
		{
			const BasePtr* basePtr = m_graphStack.back();
			m_graphStack.pop_back();

			basePtr->Mark();

			const Reflection::TypeInfo* pureType = basePtr->GetPureType();
			const Reflection::TypeInfo::PropertyMap& properties = pureType->GetProperties();

			const void* rawInstance = basePtr->GetPointer();

			for (auto [name, property] : properties)
			{
				// TODO -> How to make the property info for the general standard structure like std::vector, std::set, std::map...
				// In the future, used the ecs project, the other manager maybe use the reflection system with garbage collector.
				// But, if the manager's container is the std::vector, the contents will disappear after that the GC collecting.
				const Reflection::TypeInfo* propertyType = property->GetPropertyType();
				if (!Reflection::IsChild(baseType, propertyType))
				{
					continue;
				}

				const BasePtr* rawProperty = static_cast<const BasePtr*>(property->GetRaw(rawInstance));

				m_graphStack.push_back(rawProperty);
			}

			if (!m_timeLimit.HasTime())
			{
				return;
			}
		}
	}

	void GarbageCollector::Scan()
	{
		if (!m_graphStack.empty())
		{
			return;
		}

		LOGINFO() << "[COLLECTOR] GC Cycle - Scan";

		GetStorage().Scan();

		m_status = eStatus::eSweeping;
	}

	void GarbageCollector::Sweep()
	{
		LOGINFO() << "[COLLECTOR] GC Cycle - Sweep";

		if (GetStorage().Sweep(m_timeLimit))
		{
			m_status = eStatus::eIdle;
		}
		else
		{
			m_status = eStatus::eSweeping;
		}
	}
};