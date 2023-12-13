#pragma once

namespace CE
{
    
	class CORE_API IAllocator
	{
	public:
		using SizeType = SIZE_T;
		using AlignType = SIZE_T;

		IAllocator() = default;
		virtual ~IAllocator() = default;

		virtual void* Allocate(SizeType byteSize) = 0;

		virtual void Free(void* block, SizeType size = 0) = 0;

		virtual void* AlignedAlloc(SizeType byteSize, AlignType alignment) = 0;
		virtual void AlignedFree(void* block, SizeType size = 0) = 0;

	};

} // namespace CE
