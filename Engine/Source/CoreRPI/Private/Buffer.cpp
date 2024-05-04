#include "CoreRPI.h"

namespace CE::RPI
{
	Buffer::Buffer(const RHI::BufferDescriptor& bufferDescriptor)
	{
		for (int i = 0; i < buffers.GetSize(); ++i)
		{
			buffers[i] = gDynamicRHI->CreateBuffer(bufferDescriptor);
		}
	}

	Buffer::~Buffer()
	{
		for (int i = 0; i < buffers.GetSize(); ++i)
		{
			delete buffers[i]; buffers[i] = nullptr;
		}
	}

	void Buffer::UploadData(u32 imageIndex, u64 dataSize, u64 startOffsetInBuffer, const void* data)
	{
		buffers[imageIndex]->UploadData(data, dataSize, startOffsetInBuffer);
	}

	void Buffer::UploadData(u32 imageIndex, const RHI::BufferData& uploadData)
	{
		buffers[imageIndex]->UploadData(uploadData);
	}

	void Buffer::Map(u32 imageIndex, u64 offset, u64 byteSize, void** outDataPtr)
	{
		buffers[imageIndex]->Map(offset, byteSize, outDataPtr);
	}

	void Buffer::Unmap(u32 imageIndex)
	{
		buffers[imageIndex]->Unmap();
	}

} // namespace CE::RPI
