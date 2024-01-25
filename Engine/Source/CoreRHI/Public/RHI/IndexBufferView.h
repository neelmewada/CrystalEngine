#pragma once

namespace CE::RHI
{
    
	class CORERHI_API IndexBufferView final
	{
	public:

		IndexBufferView() = default;

		IndexBufferView(RHI::Buffer* buffer, u64 byteOffset, u64 byteCount, IndexFormat indexFormat);

		inline RHI::Buffer* GetBuffer() const
		{
			return buffer;
		}

		inline u64 GetByteOffset() const
		{
			return byteOffset;
		}

		inline u64 GetByteCount() const
		{
			return byteCount;
		}

		inline IndexFormat GetIndexFormat() const
		{
			return indexFormat;
		}

	private:

		RHI::Buffer* buffer = nullptr;
		u64 byteOffset = 0;
		u64 byteCount = 0;
		IndexFormat indexFormat = IndexFormat::Uint16;

	};

} // namespace CE::RHI
