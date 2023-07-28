#include "CoreMinimal.h"

namespace CE
{
	Stream& operator<<(Stream& stream, BlobMetaData& metaData)
	{
		if (stream.CanWrite())
		{
			stream << metaData.size;
			stream << metaData.offsetInFile;
			stream << (u32)metaData.flags;
		}

		return stream;
	}

	Stream& operator>>(Stream& stream, BlobMetaData& metaData)
	{
		if (stream.CanRead())
		{
			stream >> metaData.size;
			stream >> metaData.offsetInFile;
			u32 flags = 0;
			stream >> flags;
			metaData.flags = (BinaryBlobFlags)flags;
		}

		return stream;
	}

	BinaryBlob::BinaryBlob()
	{
		
	}

	void BinaryBlob::Free()
	{
		if (isAllocated)
		{
			Memory::Free(data);
		}

		data = nullptr;
		dataSize = 0;
	}

	void BinaryBlob::Reserve(u32 byteSize)
	{
		if (dataSize >= byteSize)
		{
			return;
		}

		u8* copy = (u8*)Memory::Malloc(byteSize);
		memset(copy, 0, byteSize);
		memcpy(copy, data, dataSize);

		if (isAllocated)
		{
			Free();
		}

		data = copy;
		dataSize = byteSize;
	}

	bool BinaryBlob::IsValid()
	{
		return data != nullptr && dataSize > 0;
	}

	bool BinaryBlob::ReadFrom(Stream* stream)
	{
		if (stream == nullptr || !stream->IsOpen())
			return false;

		*stream >> metaData;
		
		if (metaData.size == 0)
		{

			return true;
		}
	}

} // namespace CE

CE_RTTI_POD_IMPL(CE, BinaryBlob)
