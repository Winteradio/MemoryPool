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
		m_graphStack.Reserve(4096);

		m_timeLimit.Init(sweepTime);
	}

	void GarbageCollector::AddRoot(const BasePtr* rootPtr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_rootSet.Insert(rootPtr);
	}

	void GarbageCollector::RemoveRoot(const BasePtr* rootPtr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_rootSet.Erase(rootPtr);
	}

	void GarbageCollector::Collect()
	{
		LOGINFO() << "[COLLECTOR] Start the GC Cycle";

		m_timeLimit.Start();

		if (eStatus::eIdle == m_status)
		{
			Prepare();
		}

		if (eStatus::eMarking == m_status)
		{
			Mark();
		}

		if (eStatus::eSweeping == m_status)
		{
			Sweep();
		}

		if (eStatus::ePurging == m_status)
		{
			Purge();
		}
	}

	void GarbageCollector::Prepare()
	{
		LOGINFO() << "[COLLECTOR] GC Cycle - Prepare";

		std::lock_guard<std::mutex> lock(m_mutex);

		for (const BasePtr* basePtr : m_rootSet)
		{
			m_graphStack.PushBack(basePtr);
		}

		m_status = eStatus::eMarking;
	}

	void GarbageCollector::Mark()
	{
		static const Reflection::TypeInfo* baseType = Reflection::TypeInfo::Get<BasePtr>();

		LOGINFO() << "[COLLECTOR] GC Cycle - Mark";

		while (!m_graphStack.Empty())
		{
			const BasePtr* basePtr = m_graphStack.Back();
			m_graphStack.PopBack();

			if (basePtr->IsMarked())
			{
				continue;
			}

			basePtr->Mark();

			const Reflection::TypeInfo* pureType = basePtr->GetPureType();
			const Reflection::TypeInfo::PropertyMap& properties = pureType->GetProperties();

			const void* rawInstance = basePtr->GetPointer();

			for (auto [name, property] : properties)
			{
				const void* rawProperty = property->GetRaw(rawInstance);

				if (const Reflection::ArrayPropertyInfo* arrayProperty = Reflection::Cast<const Reflection::ArrayPropertyInfo*>(property))
				{
					const Reflection::TypeInfo* valueType = arrayProperty->GetValueType();
					if (!Reflection::IsChild(baseType, valueType))
					{
						continue;
					}

					auto beginItr = arrayProperty->begin(rawProperty);
					auto endItr = arrayProperty->end(rawProperty);
					
					for (auto itr = beginItr; itr != endItr; itr++)
					{
						const BasePtr* rawValue = static_cast<const BasePtr*>(itr.get());
						m_graphStack.PushBack(rawValue);
					}
				}
				else if (const Reflection::SetPropertyInfo* setProperty = Reflection::Cast<const Reflection::SetPropertyInfo*>(property))
				{
					const Reflection::TypeInfo* valueType = setProperty->GetValueType();
					if (!Reflection::IsChild(baseType, valueType))
					{
						continue;
					}

					auto beginItr = arrayProperty->begin(rawProperty);
					auto endItr = arrayProperty->end(rawProperty);

					for (auto itr = beginItr; itr != endItr; itr++)
					{
						const BasePtr* rawValue = static_cast<const BasePtr*>(itr.get());
						m_graphStack.PushBack(rawValue);
					}
				}
				else if (const Reflection::MapPropertyInfo* mapProperty = Reflection::Cast<const Reflection::MapPropertyInfo*>(property))
				{
					const Reflection::TypeInfo* keyType = mapProperty->GetKeyType();
					const Reflection::TypeInfo* mappedType = mapProperty->GetMappedType();
					
					const bool keyMarked = Reflection::IsChild(baseType, keyType);
					const bool mappedMarked = Reflection::IsChild(baseType, mappedType);
					if (!keyMarked && !mappedMarked)
					{
						continue;
					}

					auto beginItr = mapProperty->begin(rawProperty);
					auto endItr = mapProperty->end(rawProperty);

					for (auto itr = beginItr; itr != endItr; itr++)
					{
						const void* rawValue = itr.get();
						if (keyMarked)
						{
							const BasePtr* rawKey = static_cast<const BasePtr*>(mapProperty->GetRawKey(rawValue));
							m_graphStack.PushBack(rawKey);
						}

						if (mappedMarked)
						{
							const BasePtr* rawMapped = static_cast<const BasePtr*>(mapProperty->GetRawMapped(rawValue));
							m_graphStack.PushBack(rawMapped);
						}
					}
				}
				else
				{
					const Reflection::TypeInfo* propertyType = property->GetPropertyType();
					if (!Reflection::IsChild(baseType, propertyType))
					{
						continue;
					}

					const BasePtr* rawValue = static_cast<const BasePtr*>(rawProperty);
					m_graphStack.PushBack(rawValue);
				}
			}
		}

		m_status = eStatus::eSweeping;
	}

	void GarbageCollector::Sweep()
	{
		LOGINFO() << "[COLLECTOR] GC Cycle - Sweep";

		GetStorage().Sweep();

		m_status = eStatus::ePurging;
	}

	void GarbageCollector::Purge()
	{
		LOGINFO() << "[COLLECTOR] GC Cycle - Purge";

		m_timeLimit.SetInterval(10);

		if (GetStorage().Purge(m_timeLimit))
		{
			LOGINFO() << "[COLLECTOR] Done the GC Cycle";

			m_status = eStatus::eIdle;
		}
		else
		{
			m_status = eStatus::ePurging;
		}
	}
};