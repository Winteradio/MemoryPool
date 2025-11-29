#ifndef __MEMORY_STORAGEMANAGER_H__
#define __MEMORY_STORAGEMANAGER_H__

#include "Storage/PoolBudget.h"
#include "Storage/ArrayBudget.h"
#include "Accessor/Accessor.h"

#include "Reflection/include/Reflection.h"

#include <unordered_map>
#include <mutex>

namespace Memory
{
	class TimeLimit;

	class StorageManager
	{
		public :
			using PoolBudgetMap = std::unordered_map<size_t, PoolBudget>;
			using ArrayBudgetMap = std::unordered_map<size_t, ArrayBudget>;

		public :
			StorageManager();
			~StorageManager();

		public :
			void Init(const size_t poolSize);
			void Release();

			void Scan();
			bool Sweep(TimeLimit& timeLimit);

		public :
			template<typename T, typename... Args>
			Accessor<T>* Create(Args&&... args)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				PoolBudget& poolBudget = GetPool<T>();
				PoolBudget::PoolEntry poolEntry = poolBudget.GetPool();
				if (nullptr == poolEntry.pool)
				{
					return nullptr;
				}

				Accessor<T>* accessor = static_cast<Accessor<T>*>(poolEntry.pool->Acquire());
				if (nullptr == accessor)
				{
					return nullptr;
				}

				accessor->Construct(std::forward<Args>(args)...);
				poolBudget.UpdatePool(poolEntry);
				return accessor;
			}

			template<typename T, typename... Args>
			Accessor<T>* CreateArray(const size_t count, Args&&... args)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				ArrayBudget& arrayBudget = GetArray<T>();
				ArrayBudget::ArrayEntry arrayEntry = arrayBudget.GetArray(count);
				if (nullptr == arrayEntry.array)
				{
					return nullptr;
				}

				Accessor<T>* accessor = static_cast<Accessor<T>*>(arrayEntry.array->Acquire());
				if (nullptr == accessor)
				{
					return nullptr;
				}
				accessor->Construct(std::forward<Args>(args)...);

				return accessor;
			}


		private :
			template<typename T>
			PoolBudget& GetPool()
			{
				const Reflection::TypeInfo* typeInfo = Reflection::TypeInfo::Get<T>();
				const size_t typeHash = typeInfo->GetTypeHash();

				auto [itr, inserted] = m_poolMap.try_emplace(typeHash);
				auto& PoolBudget = itr->second;
				if (inserted)
				{
					PoolBudget.Init<T>(m_poolSize);
				}

				return PoolBudget;
			}

			template<typename T>
			ArrayBudget& GetArray()
			{
				const Reflection::TypeInfo* typeInfo = Reflection::TypeInfo::Get<T>();
				const size_t typeHash = typeInfo->GetTypeHash();

				auto [itr, inserted] = m_arrayMap.try_emplace(typeHash);
				auto& ArrayBudget = itr->second;
				if (inserted)
				{
					ArrayBudget.Init<T>();
				}

				return ArrayBudget;
			}

		private :
			friend class GarbageCollector;

			PoolBudgetMap m_poolMap;
			ArrayBudgetMap m_arrayMap;

			std::mutex m_mutex;
			size_t m_poolSize;
	};
};

#endif // __MEMORY_STORAGEMANAGER_H__