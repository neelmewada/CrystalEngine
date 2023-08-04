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
		void Reserve(u64 byteSize);

		bool IsValid();
        
        void LoadData(const void* data, u64 dataSize);

		/// Loads raw binary blob from stream
		void LoadData(Stream* fromStream);

		bool Deserialize(Stream* stream);
        bool Serialize(Stream* stream);

		inline u8* GetDataPtr() const { return data; }
		inline u64 GetDataSize() const { return dataSize; }

		friend inline Stream& operator<<(Stream& stream, BinaryBlob& blob)
		{
			blob.Serialize(&stream);
			return stream;
		}

		friend inline Stream& operator>>(Stream& stream, BinaryBlob& blob)
		{
			blob.Deserialize(&stream);
			return stream;
		}

	private:

		b8 isAllocated = false;
        

		u8* data = nullptr;
		u64 dataSize = 0;
	};
    
} // namespace CE

CE_RTTI_POD(CORE_API, CE, BinaryBlob)
