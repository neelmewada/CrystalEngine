#pragma once

struct zip_t;

namespace CE
{

    class CORE_API ArchiveStream : private Stream
    {
    public:
        enum Mode
        {
            MODE_Read = 'r',
            MODE_Write = 'w',
            MODE_Append = 'a'
        };

        ArchiveStream(const IO::Path& archivePath, ArchiveMode mode);
        virtual ~ArchiveStream();

        s64 GetNumEntries();

        bool EntryExists(const IO::Path& entryPath);

        s32 OpenEntry(const IO::Path& entryPath);
        s32 OpenEntry(SIZE_T index);

        u32 ReadEntryCrc32();

        String ReadEntryName();
        // The uncompressed size of the entry
        u64 ReadEntrySize();
        // The compressed size of the entry
        u64 ReadEntryCompressedSize();

        s64 ReadEntryData(void* buffer, SIZE_T bufferSize);
        s64 ReadEntryDataAlloc(void** buffer, SIZE_T* bufferSize);

        s32 WriteEntryData(const void* buffer, SIZE_T bufferSize);

        s32 CloseEntry();

        // Overrides

        virtual void Write(const void* inData, u64 length) override;

        virtual s64 Read(void* outData, u64 length) override;

        virtual u64 GetCurrentPosition() override;

        virtual void Seek(s64 seekPos, SeekMode seekMode) override;

        virtual bool IsOutOfBounds() override;
        virtual void SetOutOfBounds() override;

        virtual bool IsOpen() override;

        virtual void Close() override;

        virtual u64 GetLength() override;

        virtual u64 GetCapacity() override;

    private:
        IO::Path archivePath{};
        ArchiveMode mode{};

        zip_t* handle = nullptr;
    };
    
} // namespace CE

