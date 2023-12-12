#pragma once

namespace CE::RHI
{
    
	class CORERHI_API IndexBufferView final
	{
	public:

		IndexBufferView() = default;

		IndexBufferView(RHI::Buffer* buffer, u32 byteOffset, u32 byteCount, IndexFormat indexFormat);

		inline RHI::Buffer* GetBuffer() const
		{
			return buffer;
		}

		inline u32 GetByteOffset() const
		{
			return byteOffset;
		}

		inline u32 GetByteCount() const
		{
			return byteCount;
		}

		inline IndexFormat GetIndexFormat() const
		{
			return indexFormat;
		}

	private:

		RHI::Buffer* buffer = nullptr;
		u32 byteOffset = 0;
		u32 byteCount = 0;
		IndexFormat indexFormat = IndexFormat::Uint16;

	};

} // namespace CE::RHI
