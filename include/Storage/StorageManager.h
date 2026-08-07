#ifndef __MEMORY_STORAGEMANAGER_H__
#define __MEMORY_STORAGEMANAGER_H__

#include "Storage/PoolBudget.h"
#include "Storage/ArrayBudget.h"
#include "Accessor/Accessor.h"

#include "Reflection/include/Reflection.h"
#include "Container/include/HashMap.h"

#include <mutex>
#include <unordered_map>

namespace Memory
{
	class TimeLimit;

	class StorageManager
	{
		public :
			using PoolBudgetMap = wtr::HashMap<size_t, PoolBudget>;
			using ArrayBudgetMap = wtr::HashMap<size_t, ArrayBudget>;

		public :
			StorageManager();
			~StorageManager();

		public :
			void Init(const size_t poolSize);
			void Release();
			void Clear();

			void Prepare();
			void Sweep();
			bool Purge(TimeLimit& timeLimit);

		public :
			template<typename T, typename... Args>
			Accessor<T>* Create(Args&&... args)
			{
				PoolBudget::PoolEntry poolEntry;
				Accessor<T>* accessor = nullptr;
				{
					std::lock_guard<std::mutex> lock(m_mutex);

					PoolBudget& poolBudget = GetPool<T>();
					poolEntry = poolBudget.GetPool();
					if (nullptr == poolEntry.pool)
					{
						return nullptr;
					}

					accessor = static_cast<Accessor<T>*>(poolEntry.pool->Acquire());
					if (nullptr == accessor)
					{
						return nullptr;
					}

					if (poolBudget.CheckDensity(poolEntry))
					{
						poolBudget.UpdatePool(poolEntry);
					}
				}

				accessor->Construct(std::forward<Args>(args)...);

				return accessor;
			}

			template<typename T, typename... Args>
			Accessor<T>* CreateArray(const size_t count, Args&&... args)
			{
				ArrayBudget::ArrayEntry arrayEntry;
				Accessor<T>* accessor = nullptr;

				{
					std::lock_guard<std::mutex> lock(m_mutex);

					ArrayBudget& arrayBudget = GetArray<T>();
					arrayEntry = arrayBudget.GetArray(count);
					if (nullptr == arrayEntry.array)
					{
						return nullptr;
					}

					accessor = static_cast<Accessor<T>*>(arrayEntry.array->Acquire());
					if (nullptr == accessor)
					{
						return nullptr;
					}
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

				auto [itr, inserted] = m_poolMap.TryEmplace(typeHash);
				auto& poolBudget = itr->second;
				if (inserted)
				{
					poolBudget.Init<T>(m_poolSize, typeInfo->GetTypeName());
				}

				return poolBudget;
			}

			template<typename T>
			ArrayBudget& GetArray()
			{
				const Reflection::TypeInfo* typeInfo = Reflection::TypeInfo::Get<T>();
				const size_t typeHash = typeInfo->GetTypeHash();

				auto [itr, inserted] = m_arrayMap.TryEmplace(typeHash);
				auto& arrayBudget = itr->second;
				if (inserted)
				{
					arrayBudget.Init<T>(typeInfo->GetTypeName());
				}

				return arrayBudget;
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