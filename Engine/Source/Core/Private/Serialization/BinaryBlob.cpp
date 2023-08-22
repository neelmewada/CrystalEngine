#include "CoreMinimal.h"

namespace CE
{
	

	BinaryBlob::BinaryBlob()
	{
		
	}

	BinaryBlob::BinaryBlob(const BinaryBlob& copy)
	{
		Free();

		if (copy.IsValid())
		{
			LoadData(copy.data, copy.dataSize);
		}
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

	void BinaryBlob::Reserve(u64 byteSize)
	{
		u8* copy = (u8*)Memory::Malloc(byteSize);
		memset(copy, 0, byteSize);
        if (data != nullptr && dataSize > 0)
            memcpy(copy, data, dataSize);

        Free();

		data = copy;
		dataSize = byteSize;
	}

	bool BinaryBlob::IsValid() const
	{
		return data != nullptr && dataSize > 0;
	}

    void BinaryBlob::LoadData(const void* data, u64 dataSize)
    {
        Reserve(dataSize);
        memcpy(this->data, data, dataSize);
    }

	void BinaryBlob::LoadData(Stream* fromStream)
	{
		if (!fromStream->CanRead() || !fromStream->IsOpen())
			return;

		fromStream->SetBinaryMode(true);

		u64 length = fromStream->GetLength();
		Free();

		if (length > 0)
		{
			Reserve(length);
			fromStream->Read(data, dataSize);
		}
	}

	bool BinaryBlob::Deserialize(Stream* stream)
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

		u64 flagsValue = 0;
		*stream >> flagsValue;
		this->flags = (BinaryBlobFlags)flagsValue;
        
        Reserve((u64)size);
        
        stream->Read(data, (u64)size);
        
        return true;
	}

    bool BinaryBlob::Serialize(Stream* stream)
    {
        if (!IsValid() || stream == nullptr || !stream->IsOpen() || !stream->CanWrite())
            return false;
        
        *stream << (s64)dataSize;

		*stream << (u64)flags;

		if (dataSize > 0)
			stream->Write(data, dataSize);

        return true;
    }

} // namespace CE

CE_RTTI_POD_IMPL(CE, BinaryBlob)
