#include "Container/Arena.h"

#include <cassert>
#include <new>

namespace Memory
{
	Arena::Arena()
	{};

	Arena::~Arena()
	{}

	void* Arena::Allocate(const size_t memorySize)
	{
		void* memory = ::operator new(memorySize, std::nothrow);

		return memory;
	}

	void Arena::Deallocate(void* pointer)
	{
		if (nullptr == pointer)
		{
			return;
		}

		::operator delete(pointer);
	}
}