
#include "CoreMinimal.h"

#include "zip.h"

namespace CE
{
    
    ArchiveStream::ArchiveStream(const IO::Path& archivePath, ArchiveMode mode)
        : archivePath(archivePath), mode(mode)
    {
        String path = archivePath.GetString();
        handle = zip_open(path.GetCString(), ZIP_DEFAULT_COMPRESSION_LEVEL, (char)mode);
    }

    ArchiveStream::~ArchiveStream()
    {
        ArchiveStream::Close();
    }

    s64 ArchiveStream::GetNumEntries()
    {
        if (!IsOpen())
            return 0;

        return zip_entries_total(handle);
    }

    bool ArchiveStream::EntryExists(const IO::Path& entryPath)
    {
        return false;
    }

    s32 ArchiveStream::OpenEntry(const IO::Path& entryPath)
    {
        String entryPathStr = entryPath.GetString();
        return zip_entry_open(handle, entryPathStr.GetCString());
    }

    s32 ArchiveStream::OpenEntry(SIZE_T index)
    {
        return zip_entry_openbyindex(handle, index);
    }

    u32 ArchiveStream::ReadEntryCrc32()
    {
        return zip_entry_crc32(handle);
    }

    String ArchiveStream::ReadEntryName()
    {
        return zip_entry_name(handle);
    }

    u64 ArchiveStream::ReadEntrySize()
    {
        return zip_entry_uncomp_size(handle);
    }

    u64 ArchiveStream::ReadEntryCompressedSize()
    {
        return zip_entry_comp_size(handle);
    }

    s64 ArchiveStream::ReadEntryData(void* buffer, SIZE_T bufferSize)
    {
        return zip_entry_noallocread(handle, buffer, bufferSize);
    }

    s64 ArchiveStream::ReadEntryDataAlloc(void** buffer, SIZE_T* bufferSize)
    {
        return (s64)zip_entry_read((struct zip_t*)handle, buffer, (size_t*)bufferSize);
    }

    s32 ArchiveStream::WriteEntryData(const void* buffer, SIZE_T bufferSize)
    {
        return zip_entry_write(handle, buffer, bufferSize);
    }

    s32 ArchiveStream::CloseEntry()
    {
        return zip_entry_close(handle);
    }

    void ArchiveStream::Write(const void* inData, u64 length)
    {
        zip_entry_write(handle, inData, length);
    }

    s64 ArchiveStream::Read(void* outData, u64 length)
    {
        return zip_entry_noallocread(handle, outData, length);
    }

    u64 ArchiveStream::GetCurrentPosition()
    {
        return 0;
    }

    void ArchiveStream::Seek(s64 seekPos, SeekMode seekMode)
    {

    }

    bool ArchiveStream::IsOutOfBounds()
    {
        return false;
    }

    void ArchiveStream::SetOutOfBounds()
    {

    }

    bool ArchiveStream::IsOpen()
    {
        return handle != nullptr;
    }

    void ArchiveStream::Close()
    {
        if (IsOpen())
            zip_close(handle);
        handle = nullptr;
    }

    u64 ArchiveStream::GetLength()
    {
        return u64();
    }

    u64 ArchiveStream::GetCapacity()
    {
        return u64();
    }

} // namespace CE
