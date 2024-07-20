
#include "Memory/Memory.h"

namespace CE
{
	std::atomic<SIZE_T> Memory::totalAllocation = 0;

}

void* operator new(std::size_t size) noexcept(false)
{
	CE::Memory::totalAllocation += size;
	return malloc(size);
}

void operator delete(void* p) noexcept
{
	if (p != nullptr)
	{
		CE::Memory::totalAllocation -= _msize(p);
	}
	free(p);
}
