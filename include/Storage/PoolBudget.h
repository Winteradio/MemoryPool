#ifndef __MEMORY_POOLBUDGET_H__
#define __MEMORY_POOLBUDGET_H__

#include "Container/List.h"
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

			struct PoolEntry
			{
				List<IPool*>::Iterator handle;
				IPool* pool;
				eDensity type;

				PoolEntry();
			};

			using PoolCreater = IPool *(*)();
			
		public :
			PoolBudget();
			PoolBudget(const PoolBudget& other) = delete;
			explicit PoolBudget(PoolBudget&& other) noexcept;
			~PoolBudget();

		public :
			template<typename T>
			void Init(const size_t poolSize)
			{
				m_poolCreater = []() -> IPool*
				{
					IPool* pool = new Pool<T>();

					return pool;
				};

				m_poolSize = poolSize;
			}

		public :
			PoolEntry GetPool();
			PoolEntry AddPool(IPool* pool);

			void RemovePool(PoolEntry& poolEntry);
			void UpdatePool(PoolEntry& poolEntry);
			
			void Release();
			void Update();
			void Scan();
			bool Sweep(TimeLimit& timeLimit);

		private :
			IPool* CreatePool();
			eDensity GetDensity(const IPool* pool);

		private :
			List<IPool*> m_poolList[static_cast<size_t>(eDensity::eMax)];
			PoolCreater m_poolCreater;
			size_t m_poolSize;
	};
};

#endif // __MEMORY_POOLBUDGET_H__