#pragma once

namespace CE::RHI
{

	class CORERHI_API VertexBufferView final
	{
	public:

		VertexBufferView() = default;

		VertexBufferView(RHI::Buffer* buffer, u64 byteOffset, u64 byteCount, u64 vertexStride);

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

		inline u64 GetVertexStride() const
		{
			return vertexStride;
		}

		inline bool operator==(const VertexBufferView& other) const
		{
			return buffer == other.buffer && byteOffset == other.byteCount && byteCount == other.byteCount && vertexStride == other.vertexStride;
		}

		inline bool operator!=(const VertexBufferView& other) const
		{
			return !(*this == other);
		}

	private:

		RHI::Buffer* buffer = nullptr;
		u64 byteOffset = 0;
		u64 byteCount = 0;
		u64 vertexStride = 0;

	};

} // namespace CE::RHI
