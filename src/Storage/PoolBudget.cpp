#include "Storage/PoolBudget.h"

#include "Log/include/Log.h"
#include "TimeLimit.h"

namespace Memory
{
	PoolBudget::PoolEntry::PoolEntry()
		: handle(nullptr)
		, pool(nullptr)
		, type(eDensity::eMax)
	{
	}

	PoolBudget::PoolBudget()
		: m_poolList()
		, m_poolCreater(nullptr)
		, m_poolSize(0)
		, m_typeName()
	{}

	PoolBudget::PoolBudget(PoolBudget&& other) noexcept
		: m_poolCreater(other.m_poolCreater)
		, m_poolSize(std::move(other.m_poolSize))
		, m_typeName(std::move(other.m_typeName))
	{
		const size_t endIndex = static_cast<size_t>(eDensity::eMax);
		for (size_t index = 0; index < endIndex; index++)
		{
			m_poolList[index] = std::move(other.m_poolList[index]);
		}
	}

	PoolBudget& PoolBudget::operator=(PoolBudget&& other) noexcept
	{
		if (this != &other)
		{
			m_poolCreater = other.m_poolCreater;
			m_poolSize = std::move(other.m_poolSize);
			m_typeName = std::move(other.m_typeName);

			const size_t endIndex = static_cast<size_t>(eDensity::eMax);
			for (size_t index = 0; index < endIndex; index++)
			{
				m_poolList[index] = std::move(other.m_poolList[index]);
			}
		}

		return *this;
	}

	PoolBudget::~PoolBudget()
	{
		bool empty = false;

		const size_t endIndex = static_cast<size_t>(eDensity::eFull);
		for (size_t index = 0; index <= endIndex; index++)
		{
			auto& poolList = m_poolList[index];
			if (poolList.Empty())
			{
				empty = true;

				break;
			}
		}

		if (!empty)
		{
			Release();
		}
	}

	PoolBudget::PoolEntry PoolBudget::GetPool()
	{
		const size_t dense = static_cast<size_t>(eDensity::eDense);
		const size_t sparse = static_cast<size_t>(eDensity::eSparse);

		size_t index = dense;
		while (true)
		{
			auto& poolList = m_poolList[index];
			if (!poolList.Empty())
			{
				PoolEntry poolEntry;
				poolEntry.handle = poolList.Begin();
				poolEntry.pool = poolList.Front();
				poolEntry.type = static_cast<eDensity>(index);

				return poolEntry;
			}

			if (index == sparse)
			{
				break;
			}

			index--;
		}

		IPool* newPool = CreatePool();
		if (nullptr == newPool)
		{
			return PoolEntry();
		}

		PoolEntry newPoolEntry = AddPool(newPool);

		return newPoolEntry;
	}

	PoolBudget::PoolEntry PoolBudget::AddPool(IPool* pool)
	{
		if (nullptr == pool)
		{
			return PoolEntry();
		}

		const eDensity type = GetDensity(pool);
		if (eDensity::eMax == type)
		{
			return PoolEntry();
		}

		const size_t index = static_cast<size_t>(type);

		auto& poolList = m_poolList[index];
		
		PoolEntry poolEntry;
		poolEntry.handle = poolList.Insert(poolList.Begin(), pool);
		poolEntry.pool = pool;
		poolEntry.type = type;

		return poolEntry;
	}

	PoolBudget::PoolIterator PoolBudget::RemovePool(const PoolEntry& poolEntry)
	{
		if (nullptr == poolEntry.pool || eDensity::eMax == poolEntry.type)
		{
			return poolEntry.handle;
		}

		const size_t index = static_cast<size_t>(poolEntry.type);

		auto& poolList = m_poolList[index];
		if (poolList.End() == poolEntry.handle)
		{
			return poolList.End();
		}

		LOGINFO() << "[BUDGET] Remove the pool(" << poolEntry.pool << ") "
			<< " | type : " << m_typeName
			<< " | size : " << poolEntry.pool->GetTotalSize()
			<< " | chunk : " << poolEntry.pool->GetChunkSize()
			<< " | count : " << poolEntry.pool->GetTotalCount();

		return poolList.Erase(poolEntry.handle);
	}

	void PoolBudget::UpdatePool(PoolEntry& poolEntry)
	{
		if (nullptr == poolEntry.pool)
		{
			return;
		}

		const eDensity eCurrentType = GetDensity(poolEntry.pool);
		const eDensity eOriginType = poolEntry.type;

		const size_t origin = static_cast<size_t>(eOriginType);
		const size_t current = static_cast<size_t>(eCurrentType);

		auto& originList = m_poolList[origin];
		auto& currentList = m_poolList[current];

		if (originList.End() == poolEntry.handle)
		{
			return;
		}

		currentList.Splice(currentList.Begin(), originList, poolEntry.handle);

		poolEntry.type = eCurrentType;

		LOGINFO() << "[BUCKET] Update the pool(" << poolEntry.pool << ") "
			<< " | type : " << m_typeName 
			<< " | updated(" 
			<< densityArray[static_cast<size_t>(eOriginType)].second << "->"
			<< densityArray[static_cast<size_t>(eCurrentType)].second << ")";

		return;
	}

	void PoolBudget::Release()
	{
		LOGINFO() << "[BUDGET] Release, the pool bucket(" << this << ") | type : " << m_typeName;

		for (auto& poolList : m_poolList)
		{
			auto itr = poolList.Begin();
			while (itr != poolList.End())
			{
				IStorage* pool = *itr;
				if (nullptr != pool)
				{
					delete pool;

					*itr = nullptr;
				}

				itr = poolList.Erase(itr);
			}

			poolList.Clear();
		}
	}

	void PoolBudget::Remove()
	{
		const size_t sparseIndex = static_cast<size_t>(eDensity::eSparse);
		auto& poolList = m_poolList[sparseIndex];

		auto itr = poolList.Begin();
		while (itr != poolList.End())
		{
			auto* pool = *itr;
			if (nullptr != pool && pool->Empty())
			{
				PoolEntry poolEntry;
				poolEntry.handle = itr;
				poolEntry.pool = pool;
				poolEntry.type = eDensity::eSparse;

				itr = RemovePool(poolEntry);
			}
			else
			{
				itr++;
			}
		}
	}

	void PoolBudget::Sweep()
	{
		const size_t endIndex = static_cast<size_t>(eDensity::eMax);
		for (size_t index = 0; index < endIndex; index++)
		{
			auto& poolList = m_poolList[index];
			for (auto itr = poolList.Begin(); itr != poolList.End(); itr++)
			{
				auto* pool = *itr;
				if (nullptr == pool)
				{
					continue;
				}

				pool->Sweep();
			}
		}
	}

	bool PoolBudget::Purge(TimeLimit& timeLimit)
	{
		const size_t endIndex = static_cast<size_t>(eDensity::eMax);
		for (size_t index = 0; index < endIndex; index++)
		{
			auto& poolList = m_poolList[index];
			for (auto itr = poolList.Begin(); itr != poolList.End(); itr++)
			{
				auto* pool = *itr;
				if (nullptr == pool)
				{
					continue;
				}

				if (!pool->Purge(timeLimit))
				{
					return false;
				}
			}
		}

		return true;
	}

	void PoolBudget::Update()
	{
		const size_t endIndex = static_cast<size_t>(eDensity::eMax);
		for (size_t index = 0; index < endIndex; index++)
		{
			const eDensity density = static_cast<eDensity>(index);

			auto& poolList = m_poolList[index];
			auto itr = poolList.Begin();
			while (itr != poolList.End())
			{
				auto* pool = *itr;

				PoolEntry poolEntry;
				poolEntry.handle = itr;
				poolEntry.pool = pool;
				poolEntry.type = density;

				if (nullptr != pool && CheckDensity(poolEntry))
				{
					auto nextItr = ++itr;
					UpdatePool(poolEntry);
					itr = nextItr;
				}
				else
				{
					itr++;
				}
			}
		}
	}

	IPool* PoolBudget::CreatePool()
	{
		if (nullptr == m_poolCreater)
		{
			return nullptr;
		}

		IPool* pool = m_poolCreater();
		if (nullptr == pool)
		{
			return nullptr;
		}

		pool->Init(m_poolSize);

		LOGINFO() << "[BUDGET] Create the pool(" << pool << ") "
			<< " | type : " << m_typeName
			<< " | size : " << pool->GetTotalSize() 
			<< " | chunk : " << pool->GetChunkSize() 
			<< " | count : " << pool->GetTotalCount();

		return pool;
	}

	PoolBudget::eDensity PoolBudget::GetDensity(const IPool* pool) const
	{
		if (nullptr == pool)
		{
			return eDensity::eMax;
		}

		const float usedRatio = pool->GetUsedRatio();
		if (usedRatio >= 0.f && usedRatio < 0.3f)
		{
			return eDensity::eSparse;
		}
		else if (usedRatio >= 0.3f && usedRatio < 0.7f)
		{
			return eDensity::eModerate;
		}
		else if (usedRatio >= 0.7f && usedRatio < 1.0f)
		{
			return eDensity::eDense;
		}
		else
		{
			return eDensity::eFull;
		}
	}

	bool PoolBudget::CheckDensity(const PoolEntry& poolEntry) const
	{
		const eDensity eCurrentType = GetDensity(poolEntry.pool);
		const eDensity eOriginType = poolEntry.type;

		return !(eCurrentType == eOriginType);
	}
	
	bool PoolBudget::Empty() const
	{
		const size_t endIndex = static_cast<size_t>(eDensity::eMax);
		for (size_t index = 0; index < endIndex; index++)
		{
			auto& poolList = m_poolList[index];
			if (!poolList.Empty())
			{
				return false;
			}
		}

		return true;
	}
}