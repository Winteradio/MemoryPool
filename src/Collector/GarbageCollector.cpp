#include "Collector/GarbageCollector.h"

#include "Log/include/Log.h"
#include "Core.h"

#include <queue>

namespace Memory
{
	GarbageCollector::GarbageCollector()
		: m_rootSet()
		, m_graphStack()
		, m_timeLimit()
		, m_mutex()
		, m_status(eStatus::eIdle)
	{
		m_graphStack.Reserve(4096);
		m_timeLimit.Init(100);
	}

	GarbageCollector::~GarbageCollector()
	{}

	void GarbageCollector::Init(const size_t sweepTime)
	{
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
		LOGINFO() << "[COLLECTOR] GC Cycle - Mark";

		while (!m_graphStack.Empty())
		{
			const BasePtr* basePtr = m_graphStack.Back();
			m_graphStack.PopBack();

			if (nullptr == basePtr || !basePtr->IsValid() || basePtr->IsMarked())
			{
				continue;
			}

			basePtr->Mark();

			const Reflection::TypeInfo* runtimeType = basePtr->GetRuntimeType();
			const Reflection::TypeInfo::PropertyMap& properties = runtimeType->GetProperties();

			const void* rawInstance = basePtr->GetPointer();

			for (const auto& [name, propertyInfo] : properties)
			{
				const eDataType eType = GetDataType(propertyInfo);
				const void* rawProperty = propertyInfo->GetRaw(rawInstance);

				if (eDataType::eContainer == eType)
				{
					MarkContainer(rawProperty, propertyInfo, m_graphStack);
				}
				else if (eDataType::eStruct == eType)
				{
					MarkStruct(rawProperty, propertyInfo, m_graphStack);
				}
				else if (eDataType::eObject == eType)
				{
					MarkObject(rawProperty, propertyInfo, m_graphStack);
				}
				else
				{}
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

	void GarbageCollector::MarkContainer(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack)
	{
		static const Reflection::TypeInfo* baseType = Reflection::TypeInfo::Get<BasePtr>();

		if (const Reflection::ArrayPropertyInfo* arrayProperty = Reflection::Cast<const Reflection::ArrayPropertyInfo*>(propertyInfo))
		{
			const Reflection::TypeInfo* valueType = arrayProperty->GetValueType();
			if (!Reflection::IsChild(baseType, valueType))
			{
				return;
			}

			auto beginItr = arrayProperty->begin(rawProperty);
			auto endItr = arrayProperty->end(rawProperty);

			for (auto itr = beginItr; itr != endItr; itr++)
			{
				const BasePtr* rawValue = static_cast<const BasePtr*>(itr.get());
				if (rawValue->IsValid())
				{
					graphStack.PushBack(rawValue);
				}
			}
		}
		else if (const Reflection::SetPropertyInfo* setProperty = Reflection::Cast<const Reflection::SetPropertyInfo*>(propertyInfo))
		{
			const Reflection::TypeInfo* valueType = setProperty->GetValueType();
			if (!Reflection::IsChild(baseType, valueType))
			{
				return;
			}

			auto beginItr = setProperty->begin(rawProperty);
			auto endItr = setProperty->end(rawProperty);

			for (auto itr = beginItr; itr != endItr; itr++)
			{
				const BasePtr* rawValue = static_cast<const BasePtr*>(itr.get());
				if (rawValue->IsValid())
				{
					graphStack.PushBack(rawValue);
				}
			}
		}
		else if (const Reflection::MapPropertyInfo* mapProperty = Reflection::Cast<const Reflection::MapPropertyInfo*>(propertyInfo))
		{
			const Reflection::TypeInfo* keyType = mapProperty->GetKeyType();
			const Reflection::TypeInfo* mappedType = mapProperty->GetMappedType();

			const bool keyMarked = Reflection::IsChild(baseType, keyType);
			const bool mappedMarked = Reflection::IsChild(baseType, mappedType);
			if (!keyMarked && !mappedMarked)
			{
				return;
			}

			auto beginItr = mapProperty->begin(rawProperty);
			auto endItr = mapProperty->end(rawProperty);

			for (auto itr = beginItr; itr != endItr; itr++)
			{
				const void* rawValue = itr.get();
				if (keyMarked)
				{
					const BasePtr* rawKey = static_cast<const BasePtr*>(mapProperty->GetRawKey(rawValue));
					if (rawKey->IsValid())
					{
						graphStack.PushBack(rawKey);
					}
				}

				if (mappedMarked)
				{
					const BasePtr* rawMapped = static_cast<const BasePtr*>(mapProperty->GetRawMapped(rawValue));
					if (rawMapped->IsValid())
					{
						graphStack.PushBack(rawMapped);
					}
				}
			}
		}
		else
		{
			return;
		}
	}

	void GarbageCollector::MarkStruct(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack)
	{
		const Reflection::TypeInfo* propertyType = propertyInfo->GetPropertyType();
		const Reflection::TypeInfo::PropertyMap& propertyMap = propertyType->GetProperties();

		for (const auto& [name, memberInfo] : propertyMap)
		{
			const void* rawMember = memberInfo->GetRaw(rawProperty);

			const eDataType eType = GetDataType(memberInfo);

			if (eDataType::eContainer == eType)
			{
				MarkContainer(rawMember, memberInfo, graphStack);
			}
			else if (eDataType::eStruct == eType)
			{
				MarkStruct(rawMember, memberInfo, graphStack);
			}
			else if (eDataType::eObject == eType)
			{
				MarkObject(rawMember, memberInfo, graphStack);
			}
			else
			{
			}
		}
	}

	void GarbageCollector::MarkObject(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack)
	{
		static const Reflection::TypeInfo* baseType = Reflection::TypeInfo::Get<BasePtr>();

		const Reflection::TypeInfo* propertyType = propertyInfo->GetPropertyType();
		if (!Reflection::IsChild(baseType, propertyType))
		{
			return;
		}

		const BasePtr* rawValue = static_cast<const BasePtr*>(rawProperty);
		if (rawValue->IsValid())
		{
			graphStack.PushBack(rawValue);
		}
	}

	GarbageCollector::eDataType GarbageCollector::GetDataType(const Reflection::PropertyInfo* propertyInfo) const
	{
		const Reflection::ContainerPropertyInfo* containerInfo = Reflection::Cast<const Reflection::ContainerPropertyInfo*>(propertyInfo);
		if (nullptr != containerInfo)
		{
			return eDataType::eContainer;
		}

		const Reflection::TypeInfo* propertyType = propertyInfo->GetPropertyType();
		const Reflection::TypeInfo* baseType = Reflection::TypeInfo::Get<BasePtr>();

		if (Reflection::IsChild(baseType, propertyType))
		{
			return eDataType::eObject;
		}

		const Reflection::TypeInfo::PropertyMap& propertyMap = propertyType->GetProperties();
		if (!propertyMap.empty())
		{
			return eDataType::eStruct;
		}

		return eDataType::eNone;
	}
};