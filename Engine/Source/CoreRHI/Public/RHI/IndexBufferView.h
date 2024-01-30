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

		inline bool operator==(const IndexBufferView& other) const
		{
			return buffer == other.buffer && byteOffset == other.byteCount && byteCount == other.byteCount && indexFormat == other.indexFormat;
		}

		inline bool operator!=(const IndexBufferView& other) const
		{
			return !(*this == other);
		}

	private:

		RHI::Buffer* buffer = nullptr;
		u64 byteOffset = 0;
		u64 byteCount = 0;
		IndexFormat indexFormat = IndexFormat::Uint16;

	};

} // namespace CE::RHI
