#pragma once

namespace CE::RHI
{

	class CORERHI_API VertexBufferView final
	{
	public:

		VertexBufferView() = default;

		VertexBufferView(RHI::Buffer* buffer, u32 byteOffset, u32 byteCount, u32 vertexStride);

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

		inline u32 GetVertexStride() const
		{
			return vertexStride;
		}

	private:

		RHI::Buffer* buffer = nullptr;
		u32 byteOffset = 0;
		u32 byteCount = 0;
		u32 vertexStride = 0;

	};

} // namespace CE::RHI
