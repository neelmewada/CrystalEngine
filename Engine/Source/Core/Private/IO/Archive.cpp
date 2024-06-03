
#include "CoreMinimal.h"

#include "zip.h"

namespace CE
{

	ZipArchive::ZipArchive()
	{
		
	}

	ZipArchive::~ZipArchive()
	{
		Close();
	}

	bool ZipArchive::IsOpen()
	{
		return handle != nullptr;
	}

	SIZE_T ZipArchive::GetTotalEntries()
	{
		if (!IsOpen())
			return 0;

		return zip_entries_total(handle);
	}

	bool ZipArchive::EntryExists(const String& name)
	{
		if (OpenEntry(name) != 0)
		{
			return false;
		}
		CloseEntry();
		return true;
	}

	s32 ZipArchive::OpenEntry(const String& name)
	{
		return zip_entry_open(handle, name.GetCString());
	}

	s32 ZipArchive::OpenEntry(SIZE_T index)
	{
		return zip_entry_openbyindex(handle, index);
	}

	SSIZE_T ZipArchive::EntryReadIndex()
	{
		return zip_entry_index(handle);
	}

	u64 ZipArchive::EntryReadSize()
	{
		return zip_entry_size(handle);
	}

	String ZipArchive::EntryReadName()
	{
		return zip_entry_name(handle);
	}

	u64 ZipArchive::EntryReadCompressedSize()
	{
		return zip_entry_comp_size(handle);
	}

	SSIZE_T ZipArchive::EntryRead(void* buffer, SIZE_T bufferSize)
	{
		return zip_entry_noallocread(handle, buffer, bufferSize);
	}

	SSIZE_T ZipArchive::EntryReadAlloc(void** buffer, SIZE_T* bufferSize)
	{
		return zip_entry_read(handle, buffer, (size_t*)bufferSize);
	}

	s32 ZipArchive::EntryWrite(const void* buffer, SIZE_T bufferSize)
	{
		return zip_entry_write(handle, buffer, bufferSize);
	}

	s32 ZipArchive::CloseEntry()
	{
		return zip_entry_close(handle);
	}

	void ZipArchive::Open(IO::Path archivePath, ArchiveMode mode)
	{
		if (IsOpen())
			return;

		this->mode = mode;

		String path = archivePath.GetString();
		handle = zip_open(path.GetCString(), ZIP_DEFAULT_COMPRESSION_LEVEL, (char)mode);
	}

	void ZipArchive::Open(const void* stream, SIZE_T size, ArchiveMode mode)
	{
		if (IsOpen())
			return;

		this->mode = mode;

		handle = zip_stream_open((const char*)stream, size, ZIP_DEFAULT_COMPRESSION_LEVEL, (char)mode);
		this->stream = (char*)stream;
		this->streamSize = size;
	}

	void ZipArchive::Close()
	{
		if (!IsOpen())
			return;

		if (stream != nullptr)
		{
			if (handle != nullptr)
				zip_stream_close(handle);
			
			stream = nullptr;
			streamSize = 0;
		}
		else
		{
			if (handle != nullptr)
				zip_close(handle);
		}
		
		handle = nullptr;
	}

} // namespace CE
