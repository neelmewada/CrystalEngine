
#include "CoreMinimal.h"

#include "zip.h"

namespace CE
{

	Archive::Archive()
	{
		
	}

	Archive::~Archive()
	{
		Close();
	}

	bool Archive::IsOpen()
	{
		return handle != nullptr;
	}

	SIZE_T Archive::GetTotalEntries()
	{
		if (!IsOpen())
			return 0;

		return zip_entries_total(handle);
	}

	bool Archive::EntryExists(const String& name)
	{
		if (OpenEntry(name) != 0)
		{
			return false;
		}
		CloseEntry();
		return true;
	}

	s32 Archive::OpenEntry(const String& name)
	{
		return zip_entry_open(handle, name.GetCString());
	}

	s32 Archive::OpenEntry(SIZE_T index)
	{
		return zip_entry_openbyindex(handle, index);
	}

	SSIZE_T Archive::EntryReadIndex()
	{
		return zip_entry_index(handle);
	}

	u64 Archive::EntryReadSize()
	{
		return zip_entry_size(handle);
	}

	String Archive::EntryReadName()
	{
		return zip_entry_name(handle);
	}

	u64 Archive::EntryReadCompressedSize()
	{
		return zip_entry_comp_size(handle);
	}

	SSIZE_T Archive::EntryRead(void* buffer, SIZE_T bufferSize)
	{
		
		return zip_entry_noallocread(handle, buffer, bufferSize);
	}

	SSIZE_T Archive::EntryReadAlloc(void** buffer, SIZE_T* bufferSize)
	{
		return zip_entry_read(handle, buffer, (size_t*)bufferSize);
	}

	s32 Archive::EntryWrite(const void* buffer, SIZE_T bufferSize)
	{
		return zip_entry_write(handle, buffer, bufferSize);
	}

	s32 Archive::CloseEntry()
	{
		return zip_entry_close(handle);
	}

	void Archive::Open(IO::Path archivePath, ArchiveMode mode)
	{
		if (IsOpen())
			return;

		this->mode = mode;

		String path = archivePath.GetString();
		handle = zip_open(path.GetCString(), ZIP_DEFAULT_COMPRESSION_LEVEL, (char)mode);
	}

	void Archive::Close()
	{
		if (!IsOpen())
			return;

		if (stream != nullptr)
		{
			if (handle != nullptr)
				zip_stream_close(handle);

			delete[] stream;
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
