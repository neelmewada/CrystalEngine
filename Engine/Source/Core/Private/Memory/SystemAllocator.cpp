#include "CoreMinimal.h"

namespace CE
{
    SystemAllocator* SystemAllocator::Get()
    {
		static SystemAllocator instance{};
        return &instance;
    }

	void* SystemAllocator::Allocate(SizeType byteSize)
	{
		return malloc(byteSize);
	}

	void SystemAllocator::Free(void* block, SizeType size)
	{
		free(block);
	}

	void* SystemAllocator::AlignedAlloc(SizeType byteSize, AlignType alignment)
	{
		return Memory::AlignedAlloc(byteSize, alignment);
	}

	void SystemAllocator::AlignedFree(void* block, SizeType size)
	{
		Memory::AlignedFree(block);
	}

} // namespace CE
