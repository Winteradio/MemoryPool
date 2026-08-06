#include "Storage/ArrayBudget.h"

#include "Log/include/Log.h"
#include "TimeLimit.h"

namespace Memory
{
	ArrayBudget::ArrayEntry::ArrayEntry()
		: handle(nullptr)
		, array(nullptr)
	{}

	ArrayBudget::ArrayBudget()
		: m_arrayList()
		, m_arrayCreater()
		, m_typeName()
	{}

	ArrayBudget::ArrayBudget(ArrayBudget&& other) noexcept
		: m_arrayList(std::move(other.m_arrayList))
		, m_arrayCreater(std::move(other.m_arrayCreater))
		, m_typeName(std::move(other.m_typeName))
	{}

	ArrayBudget& ArrayBudget::operator=(ArrayBudget&& other) noexcept
	{
		if (this != &other)
		{
			m_arrayList = std::move(other.m_arrayList);
			m_arrayCreater = std::move(other.m_arrayCreater);
			m_typeName = std::move(other.m_typeName);
		}

		return *this;
	}

	ArrayBudget::~ArrayBudget()
	{
		if (!m_arrayList.Empty())
		{
			Release();
		}
	}

	ArrayBudget::ArrayEntry ArrayBudget::GetArray(const size_t count)
	{
		ArrayEntry arrayEntry;
		arrayEntry.array = CreateArray(count);
		arrayEntry.handle = m_arrayList.Insert(m_arrayList.Begin(), arrayEntry.array);

		return arrayEntry;
	}

	ArrayBudget::ArrayIterator ArrayBudget::RemoveArray(const ArrayEntry& arrayEntry)
	{
		if (nullptr == arrayEntry.array)
		{
			return arrayEntry.handle;
		}

#ifdef DEBUG_MEMORY
		LOGINFO() << "[MEMORY] Remove the array(" << arrayEntry.array << ") "
			<< " | type : " << m_typeName
			<< " | size : " << arrayEntry.array->GetTotalSize()
			<< " | chunk : " << arrayEntry.array->GetChunkSize()
			<< " | count : " << arrayEntry.array->GetTotalCount();
#endif

		return m_arrayList.Erase(arrayEntry.handle);
	}

	void ArrayBudget::Release()
	{
#ifdef DEBUG_MEMORY
		LOGINFO() << "[BUDGET] Release, the array bucket(" << this << ") | type : " << m_typeName;
#endif

		auto itr = m_arrayList.Begin();
		while (itr != m_arrayList.End())
		{
			IStorage* array = *itr;
			if (nullptr != array)
			{
				delete array;
				*itr = nullptr;
			}

			itr = m_arrayList.Erase(itr);
		}

		m_arrayList.Clear();
	}

	void ArrayBudget::Prepare()
	{
		for (auto* array : m_arrayList)
		{
			if (nullptr != array)
			{
				array->Prepare();
			}
		}
	}

	void ArrayBudget::Sweep()
	{
		for (auto* array : m_arrayList)
		{
			if (nullptr != array)
			{
				array->Sweep();
			}
		}
	}

	bool ArrayBudget::Purge(TimeLimit& timeLimit)
	{
		for (auto* array : m_arrayList)
		{
			if (nullptr == array)
			{
				continue;
			}

			if (!array->Purge(timeLimit))
			{
				return false;
			}
		}

		return true;
	}

	void ArrayBudget::Remove()
	{
		auto itr = m_arrayList.Begin();
		while (itr != m_arrayList.End())
		{
			auto* array = *itr;
			if (nullptr != array && array->Empty())
			{
				ArrayEntry arrayEntry;
				arrayEntry.array = array;
				arrayEntry.handle = itr;

				itr = RemoveArray(arrayEntry);
			}
			else
			{
				itr++;
			}
		}
	}

	IStorage* ArrayBudget::CreateArray(const size_t count)
	{
		if (nullptr == m_arrayCreater)
		{
			return nullptr;
		}

		IStorage* array = m_arrayCreater();
		if (nullptr == array)
		{
			return nullptr;
		}

		array->Init(count);

#ifdef DEBUG_MEMORY
		LOGINFO() << "[BUDGET] Create the array(" << array << ") " 
			<< " | type : " << m_typeName
			<< " | size : " << array->GetTotalSize() 
			<< " | chunk : " << array->GetChunkSize() 
			<< " | count : " << array->GetTotalCount();
#endif

		return array;
	}

	bool ArrayBudget::Empty() const
	{
		return m_arrayList.Empty();
	}
}