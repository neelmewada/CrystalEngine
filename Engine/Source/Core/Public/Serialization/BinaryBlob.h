#pragma once

namespace CE
{
	enum BinaryBlobFlags : u32
	{
		BLOB_None = 0,
	};
	ENUM_CLASS_FLAGS(BinaryBlobFlags);

	struct CORE_API BlobMetaData
	{
		/// Size of binary blob data excluding meta data
		u32 size = 0;

		/// Offset to the blob data in the entire file/stream
		u64 offsetInFile = 0;

		/// Blob flags
		BinaryBlobFlags flags = BLOB_None;

		friend Stream& operator<<(Stream& stream, BlobMetaData& metaData);
		friend Stream& operator>>(Stream& stream, BlobMetaData& metaData);
	};

	/// Bulk data
	class CORE_API BinaryBlob
	{
	public:

		BinaryBlob();

		void Free();
		void Reserve(u32 byteSize);

		bool IsValid();

		bool ReadFrom(Stream* stream);

	private:

		b8 isAllocated = false;

		BlobMetaData metaData = {};

		u8* data = nullptr;
		u32 dataSize = 0;
	};
    
} // namespace CE

CE_RTTI_POD(CORE_API, CE, BinaryBlob)
