#pragma once

namespace CE::RHI
{
	struct TransientMemoryPoolDescriptor
	{
		u64 initialBufferPoolSize = 0;
		u64 initialTexturePoolSize = 0;
	};
    
	class CORERHI_API TransientMemoryPool final
	{
	public:

		TransientMemoryPool(const TransientMemoryPoolDescriptor& descriptor);
		virtual ~TransientMemoryPool();

	private:

		RHI::MemoryHeap* bufferPool = nullptr;
		RHI::MemoryHeap* texturePool = nullptr;

	};

} // namespace CE::RHI
