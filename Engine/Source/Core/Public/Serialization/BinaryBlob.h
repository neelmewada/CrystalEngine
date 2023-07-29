#pragma once

namespace CE
{
	enum BinaryBlobFlags : u32
	{
		BLOB_None = 0,
	};
	ENUM_CLASS_FLAGS(BinaryBlobFlags);

	

	/// Bulk data
	class CORE_API BinaryBlob
	{
	public:

		BinaryBlob();

		void Free();
		void Reserve(u32 byteSize);

		bool IsValid();
        
        void LoadData(const void* data, u32 dataSize);

		bool ReadFrom(Stream* stream);
        bool WriteTo(Stream* stream);

		inline u8* GetDataPtr() const { return data; }
		inline u32 GetDataSize() const { return dataSize; }

		friend inline Stream& operator<<(Stream& stream, BinaryBlob& blob)
		{
			blob.WriteTo(&stream);
			return stream;
		}

		friend inline Stream& operator>>(Stream& stream, BinaryBlob& blob)
		{
			blob.ReadFrom(&stream);
			return stream;
		}

	private:

		b8 isAllocated = false;
        

		u8* data = nullptr;
		u32 dataSize = 0;
	};
    
} // namespace CE

CE_RTTI_POD(CORE_API, CE, BinaryBlob)
