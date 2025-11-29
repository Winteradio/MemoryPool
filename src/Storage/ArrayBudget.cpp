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
	{}

	ArrayBudget::ArrayBudget(ArrayBudget&& other) noexcept
		: m_arrayList(std::move(other.m_arrayList))
		, m_arrayCreater(std::move(other.m_arrayCreater))
	{}

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

	void ArrayBudget::RemoveArray(ArrayEntry& arrayEntry)
	{
		if (nullptr == arrayEntry.array)
		{
			return;
		}

		LOGINFO() << "[MEMORY] Remove the array(" << arrayEntry.array << ") "
			<< " | size : " << arrayEntry.array->GetTotalSize()
			<< " | chunk : " << arrayEntry.array->GetChunkSize()
			<< " | count : " << arrayEntry.array->GetTotalCount();

		m_arrayList.Erase(arrayEntry.handle);
		arrayEntry.handle = nullptr;
		arrayEntry.array = nullptr;
	}

	void ArrayBudget::Release()
	{
		LOGINFO() << "[MEMORY] Release the array bucket(" << this << ")";

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

	bool ArrayBudget::Sweep(TimeLimit& timeLimit)
	{
		LOGINFO() << "[BUCKET] Start the array's sweep";

		auto itr = m_arrayList.Begin();
		while (itr != m_arrayList.End())
		{
			auto* array = *itr;
			if (nullptr != array && array->Sweep(timeLimit))
			{
				return false;
			}

			itr++;
		}

		LOGINFO() << "[BUCKET] Complete the array's sweep";

		return true;
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

		LOGINFO() << "[MEMORY] Create the array(" << array << ") " 
			<< " | size : " << array->GetTotalSize() 
			<< " | chunk : " << array->GetChunkSize() 
			<< " | count : " << array->GetTotalCount();

		return array;
	}
}