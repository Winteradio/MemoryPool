#ifndef __MEMORY_ARENA_H__
#define __MEMORY_ARENA_H__

#include <cstddef>

namespace Memory
{
	class Arena
	{
		public :
			Arena();
			~Arena();

		public :
			void* Allocate(const size_t memroySize);
			void Deallocate(void* pointer);
	};
};

#endif // __MEMORY_ARENA_H__