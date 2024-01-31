#pragma once

namespace CE::RHI
{
	class Buffer;

	class CORERHI_API BufferView final
	{
	public:
		BufferView() = default;

		BufferView(RHI::Buffer* buffer, u64 byteOffset = 0, u64 byteCount = 0);

		inline RHI::Buffer* GetBuffer() const { return buffer; }
		inline u64 GetByteOffset() const { return byteOffset; }
		inline u64 GetByteCount() const { return byteCount; }

	private:

		RHI::Buffer* buffer = nullptr;
		u64 byteOffset = 0;
		u64 byteCount = 0;
	};
    
} // namespace CE::RHI
