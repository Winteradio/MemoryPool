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

			using PoolCreater = IPool * (*)();
			using PoolIterator = wtr::List<IPool*>::Iterator;

			struct PoolEntry
			{
				PoolIterator handle;
				IPool* pool;
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
				m_poolCreater = []() -> IPool*
				{
					IPool* pool = new Pool<T>();

					return pool;
				};

				m_poolSize = poolSize;
				m_typeName = typeName;
			}

		public :
			PoolEntry GetPool();
			PoolEntry AddPool(IPool* pool);
			
			void Release();
			void Remove();

			void UpdatePool(PoolEntry& poolEntry);
			void Update();

			void Sweep();
			bool Purge(TimeLimit& timeLimit);

			bool Empty() const;
			bool CheckDensity(const PoolEntry& poolEntry) const;

		private :
			IPool* CreatePool();
			PoolIterator RemovePool(const PoolEntry& poolEntry);

			eDensity GetDensity(const IPool* pool) const;

		private :
			wtr::List<IPool*> m_poolList[static_cast<size_t>(eDensity::eMax)];
			PoolCreater m_poolCreater;
			size_t m_poolSize;
			std::string m_typeName;
	};
};

#endif // __MEMORY_POOLBUDGET_H__