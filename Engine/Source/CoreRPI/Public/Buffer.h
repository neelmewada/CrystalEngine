#pragma once

namespace CE::RPI
{

	class CORERPI_API Buffer final
	{
	public:

		Buffer(const RHI::BufferDescriptor& bufferDescriptor);
		~Buffer();

		void UploadData(u32 imageIndex, u64 dataSize, u64 startOffsetInBuffer, const void* data);

		void UploadData(u32 imageIndex, const RHI::BufferData& uploadData);

		void UploadData(const RHI::BufferData& uploadData)
		{
			for (int i = 0; i < buffers.GetSize(); ++i)
			{
				UploadData(i, uploadData);
			}
		}

		void UploadData(u64 dataSize, u64 startOffsetInBuffer, const void* data)
		{
			for (int i = 0; i < buffers.GetSize(); ++i)
			{
				UploadData(i, dataSize, startOffsetInBuffer, data);
			}
		}

		void Map(u32 imageIndex, u64 offset, u64 byteSize, void** outDataPtr);
		void Unmap(u32 imageIndex);

		RHI::Buffer* GetBuffer(u32 imageIndex) const { return buffers[imageIndex]; }

	private:

		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> buffers{};
	};

} // namespace CE::RPI
