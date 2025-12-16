#ifndef __MEMORY_GARBAGECOLLECTOR_H__
#define __MEMORY_GARBAGECOLLECTOR_H__

#include "TimeLimit.h"

#include <Container/include/HashSet.h>
#include <Container/include/DynamicArray.h>

#include <mutex>

namespace Reflection
{
	class PropertyInfo;
};

namespace Memory
{
	class BasePtr;

	class GarbageCollector
	{
		public :
			enum class eStatus : uint8_t
			{
				eIdle = 0,
				eMarking = 1,
				eSweeping = 2,
				ePurging = 3,
			};

			enum class eDataType : uint8_t
			{
				eNone = 0,
				eContainer = 1,
				eStruct = 2,
				eObject = 3
			};

		public :
			GarbageCollector();
			~GarbageCollector();

		public :
			void Init(const size_t sweepLimit);
			void AddRoot(const BasePtr* rootPtr);
			void RemoveRoot(const BasePtr* rootPtr);
			void Collect();

			eStatus GetStatus() const;

		private :
			void Prepare();
			void Mark();
			void Sweep();
			void Purge();
			
			void MarkContainer(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack);
			void MarkStruct(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack);
			void MarkObject(const void* rawProperty, const Reflection::PropertyInfo* propertyInfo, wtr::DynamicArray<const BasePtr*>& graphStack);

			/**
			 * @brief	Check the property's data is possible to the GC marking.
			 * @details	This function implements the checking the property's data that is possible the marking.
			 *			The supported data type is the container, object, struct.
			 *			The supported container types are the set, map, array(array, vector).
			 *			The other containers(ex. stack, queue, deque, list) are not implemented yet.
			 *			The struct type will be recursive calling marking for finding the object type.
			 *			The object type that has the relation ship with BasePtr is the main role class in the GC.
			 * @param	propertyInfo The reflected property's type.
			 * @return	Return the property's data type for the GC marking.
			*/
			eDataType GetDataType(const Reflection::PropertyInfo* propertyInfo) const;

		private :
			wtr::HashSet<const BasePtr*> m_rootSet;
			wtr::DynamicArray<const BasePtr*> m_graphStack;
			TimeLimit m_timeLimit;
			std::mutex m_mutex;
			eStatus m_status;
	};
};

#endif // __MEMORY_GARBAGECOLLECTOR_H__