#pragma once

#include "CoreTypes.h"

#include "IO/Path.h"
#include "IO/Stream/GenericStream.h"
#include "IO/Stream/MemoryStream.h"

struct zip_t;

namespace CE
{
    enum class ArchiveMode : char
    {
        Read = 'r',
        Write = 'w',
        Append = 'a',
    };
    ENUM_CLASS_FLAGS(ArchiveMode);

    /**
     * \brief A low level API for handling ZIP archives
     */
    class CORE_API Archive
    {
    public:
        Archive();
        ~Archive();

        bool IsOpen();

        // - Public API -

        void Open(IO::Path archivePath, ArchiveMode mode);
        void Close();

        CE_INLINE ArchiveMode GetMode() { return mode; }

        SIZE_T GetTotalEntries();

        bool EntryExists(const String& name);

        /// Returns 0 on success. Creates new entry if archive mode is write or append
        s32 OpenEntry(const String& name);

        /// Returns 0 on success. Creates new entry if archive mode is write or append
        s32 OpenEntry(SIZE_T index);

        /// Returns the index of currently open entry
        SSIZE_T EntryReadIndex();

        u64 EntryReadSize();
        String EntryReadName();
        u64 EntryReadCompressedSize();
        SSIZE_T EntryRead(void* buffer, SIZE_T bufferSize);
        SSIZE_T EntryReadAlloc(void** buffer, SIZE_T* bufferSize);

        s32 EntryWrite(const void* buffer, SIZE_T bufferSize);


        s32 CloseEntry();

    private:
        

    private:
        ArchiveMode mode{};
        
        char* stream = nullptr;
        SIZE_T streamSize = 0;

        zip_t* handle = nullptr;
    };
    
} // namespace CE
