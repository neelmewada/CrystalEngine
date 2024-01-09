#pragma once

namespace CE::RHI
{
	struct TransientMemoryAllocation
	{
		ResourceMemoryRequirements imagePool{};
		ResourceMemoryRequirements bufferPool{};
	};
    
	class CORERHI_API TransientMemoryPool final
	{
	public:

		TransientMemoryPool();
		virtual ~TransientMemoryPool();

	private:

		RHI::MemoryHeap* bufferPool = nullptr;
		RHI::MemoryHeap* texturePool = nullptr;

	};

} // namespace CE::RHI
