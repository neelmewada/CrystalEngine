#pragma once

namespace CE
{
    
	class CORE_API SystemAllocator : public IAllocator
	{
	private:
		SystemAllocator() = default;

	public:

		static SystemAllocator* Get();

		virtual void* Allocate(SizeType byteSize) override;

		virtual void Free(void* block, SizeType size = 0) override;

		virtual void* AlignedAlloc(SizeType byteSize, AlignType alignment) override;
		virtual void AlignedFree(void* block, SizeType size) override;

	};

} // namespace CE
