#ifndef __MEMORY_IACCESSOR_H__
#define __MEMORY_IACCESSOR_H__

#include <cstdint>

namespace Memory
{
	class IAccessor
	{
		public :
			enum class eStatus : uint8_t
			{
				eUnreachable = 0,
				eMarked = 1
			};

			IAccessor() = default;
			virtual ~IAccessor() = default;

		public :
			virtual eStatus GetStatus() const = 0;
			virtual void SetStatus(const eStatus status) const = 0;

			virtual void Destruct() = 0;

			virtual size_t GetCount() const = 0;
			virtual void* GetPointer() const = 0;
	};
};

#endif // __MEMORY_IACCESSOR_H__