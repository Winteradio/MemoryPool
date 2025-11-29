#ifndef __MEMORY_ARRAYBUDGET_H__
#define __MEMORY_ARRAYBUDGET_H__

#include "Container/List.h"
#include "Storage/Array.h"

namespace Memory
{
	class TimeLimit;

	class ArrayBudget
	{
		public :
			struct ArrayEntry
			{
				List<IStorage*>::Iterator handle;
				IStorage* array;

				ArrayEntry();
			};

			using ArrayCreater = IStorage*(*)();
			
		public :
			ArrayBudget();
			ArrayBudget(const ArrayBudget& other) = delete;
			explicit ArrayBudget(ArrayBudget&& other) noexcept;
			~ArrayBudget();

		public :
			template<typename T>
			void Init()
			{
				m_arrayCreater = []() -> IStorage*
				{
					IStorage* array = new Array<T>();

					return array;
				};
			}

		public :
			ArrayEntry GetArray(const size_t count);
			void RemoveArray(ArrayEntry& ArrayEntry);

			void Release();
			bool Sweep(TimeLimit& timeLimit);

		private :
			IStorage* CreateArray(const size_t count);

		private :
			List<IStorage*> m_arrayList;
			ArrayCreater m_arrayCreater;
	};
};

#endif // __MEMORY_ARRAYBUDGET_H__