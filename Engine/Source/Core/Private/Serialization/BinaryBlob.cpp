#include "CoreMinimal.h"

namespace CE
{
	

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
		u8* copy = (u8*)Memory::Malloc(byteSize);
		memset(copy, 0, byteSize);
        if (data != nullptr && dataSize > 0)
            memcpy(copy, data, dataSize);

        Free();

		data = copy;
		dataSize = byteSize;
	}

	bool BinaryBlob::IsValid()
	{
		return data != nullptr && dataSize > 0;
	}

    void BinaryBlob::LoadData(const void* data, u32 dataSize)
    {
        Reserve(dataSize);
        memcpy(this->data, data, dataSize);
    }

	bool BinaryBlob::ReadFrom(Stream* stream)
	{
		if (stream == nullptr || !stream->IsOpen() || !stream->CanRead())
			return false;
        
        s64 size = 0;
		*stream >> size;
        if (size < 0)
        {
            Free();
            return true;
        }
        
        Reserve((u32)size);
        
        stream->Read(data, (u32)size);
        
        return true;
	}

    bool BinaryBlob::WriteTo(Stream* stream)
    {
        if (!IsValid() || stream == nullptr || !stream->IsOpen() || !stream->CanWrite())
            return false;
        
        *stream << dataSize;
        stream->Write(data, dataSize);
        
        return true;
    }

} // namespace CE

CE_RTTI_POD_IMPL(CE, BinaryBlob)
