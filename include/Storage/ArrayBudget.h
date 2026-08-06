#ifndef __MEMORY_ARRAYBUDGET_H__
#define __MEMORY_ARRAYBUDGET_H__

#include <Container/include/List.h>

#include "Storage/Array.h"

namespace Memory
{
	class TimeLimit;

	class ArrayBudget
	{
		public :
			using ArrayCreater = IStorage * (*)();
			using ArrayIterator = wtr::List<IStorage*>::Iterator;

			struct ArrayEntry
			{
				ArrayIterator handle;
				IStorage* array;

				ArrayEntry();
			};
			
		public :
			ArrayBudget();
			ArrayBudget(const ArrayBudget& other) = delete;
			explicit ArrayBudget(ArrayBudget&& other) noexcept;
			ArrayBudget& operator=(ArrayBudget&& other) noexcept;
			~ArrayBudget();

		public :
			template<typename T>
			void Init(const std::string& typeName)
			{
				m_arrayCreater = []() -> IStorage*
				{
					IStorage* array = new Array<T>();

					return array;
				};

				m_typeName = typeName;
			}

		public :
			ArrayEntry GetArray(const size_t count);

			void Release();
			void Remove();

			void Prepare();
			void Sweep();
			bool Purge(TimeLimit& timeLimit);

			bool Empty() const;

		private :
			IStorage* CreateArray(const size_t count);
			ArrayIterator RemoveArray(const ArrayEntry& ArrayEntry);

		private :
			wtr::List<IStorage*> m_arrayList;
			ArrayCreater m_arrayCreater;

			std::string m_typeName;
	};
};

#endif // __MEMORY_ARRAYBUDGET_H__