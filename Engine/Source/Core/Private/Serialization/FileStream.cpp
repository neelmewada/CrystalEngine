
#include "CoreMinimal.h"

namespace CE
{
/*
    FileStream::FileStream(const IO::Path& path, Permissions openMode, bool isBinary)
    {
        ASSERT(openMode != Permissions::None, "FileStream constructed with openMode as None!");

        this->filePath = path;

        this->openMode = openMode;
        std::ios::openmode mode;
        if (openMode == Permissions::ReadOnly)
        {
            isLoading = true;
            mode = std::ios::in;
        }
        else if (openMode == Permissions::WriteOnly)
        {
            isLoading = false;
            mode = std::ios::out;
        }
        else if (openMode == Permissions::ReadWrite)
        {
            isLoading = false;
            mode = std::ios::in | std::ios::out;
        }
        
        if (isBinary)
            mode |= std::ios::binary;

        if ((!filePath.Exists() && isLoading) || filePath.IsDirectory())
        {
            CE_LOG(Error, All, "FileStream constructed with a path that either does not exist or is a directory: {}", filePath);
            return;
        }
        
        impl = std::fstream((fs::path)path, mode);
    }

    FileStream::~FileStream()
    {
        impl.close();
    }

    bool FileStream::IsOpen()
    {
        return impl.is_open();
    }

    void FileStream::Close()
    {
        impl.close();
    }

    bool FileStream::CanRead()
    {
        return openMode == Permissions::ReadOnly || openMode == Permissions::ReadWrite;
    }

    bool FileStream::CanWrite()
    {
        return openMode == Permissions::WriteOnly || openMode == Permissions::ReadWrite;
    }

    bool FileStream::CanResize()
    {
        return true;
    }

    bool FileStream::HasHardSizeLimit()
    {
        return false;
    }

    void FileStream::Serialize(void* value, u64 length)
    {
        if (IsLoading()) // Reading
        {
            impl.read((char*)value, length);
        }
        else // Writing
        {
            impl.write((char*)value, length);
        }
    }

    u64 FileStream::GetLength()
    {
        return impl.tellg();
    }

    u64 FileStream::GetMaximumSize()
    {
        return GetLength();
    }

    void FileStream::Seek(u32 seekPos, SeekMode dir)
    {
        std::ios_base::seekdir seekdir = std::ios::beg;
        if (dir == SeekMode::Current)
            seekdir = std::ios::cur;
        else if (dir == SeekMode::End)
            seekdir = std::ios::end;
        impl.seekg(seekPos, seekdir);
    }
    */
    
} // namespace CE

