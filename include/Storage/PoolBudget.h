#ifndef __MEMORY_POOLBUDGET_H__
#define __MEMORY_POOLBUDGET_H__

#include <Container/include/List.h>
#include <Container/include/StaticArray.h>

#include "Storage/Pool.h"

namespace Memory
{
	class TimeLimit;

	class PoolBudget
	{
		public :
			enum class eDensity : uint8_t
			{
				eSparse,
				eModerate,
				eDense,
				eFull,
				eMax
			};

			inline static wtr::StaticArray<std::pair<eDensity, const char*>, static_cast<size_t>(eDensity::eMax)> densityArray =
			{ {
				{eDensity::eSparse, "Sparse"},
				{eDensity::eModerate, "Moderate"},
				{eDensity::eDense, "Dense"},
				{eDensity::eFull, "Full"},
			} };

			using PoolCreater = IStorage * (*)();
			using PoolIterator = wtr::List<IStorage*>::Iterator;

			struct PoolEntry
			{
				PoolIterator handle;
				IStorage* pool;
				eDensity type;

				PoolEntry();
			};
			
		public :
			PoolBudget();
			PoolBudget(const PoolBudget& other) = delete;
			explicit PoolBudget(PoolBudget&& other) noexcept;
			PoolBudget& operator=(PoolBudget&& other) noexcept;
			~PoolBudget();

		public :
			template<typename T>
			void Init(const size_t poolSize, const std::string& typeName)
			{
				m_poolCreater = []() -> IStorage*
				{
					IStorage* pool = new Pool<T>();

					return pool;
				};

				m_poolSize = poolSize;
				m_typeName = typeName;
			}

		public :
			PoolEntry GetPool();
			PoolEntry AddPool(IStorage* pool);
			
			void Release();
			void Remove();

			void UpdatePool(PoolEntry& poolEntry);
			void Update();

			void Prepare();
			void Sweep();
			bool Purge(TimeLimit& timeLimit);

			bool Empty() const;
			bool CheckDensity(const PoolEntry& poolEntry) const;

		private :
			IStorage* CreatePool();
			PoolIterator RemovePool(const PoolEntry& poolEntry);

			eDensity GetDensity(const IStorage* pool) const;

		private :
			wtr::List<IStorage*> m_poolList[static_cast<size_t>(eDensity::eMax)];
			PoolCreater m_poolCreater;
			size_t m_poolSize;
			std::string m_typeName;
	};
};

#endif // __MEMORY_POOLBUDGET_H__