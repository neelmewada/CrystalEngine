
#include "CoreMinimal.h"

namespace CE
{
    FileStream::FileStream(const IO::Path& filePath, Permissions openMode, bool openAsBinary)
		: filePath(filePath)
    {
        ASSERT(openMode != Permissions::None, "FileAsciiStream constructed with openMode as None!");

        this->openMode = openMode;
        std::ios::openmode mode = 0;

        if (openMode == Permissions::ReadOnly)
        {
            mode = std::ios::in;
        }
        else if (openMode == Permissions::WriteOnly)
        {
            mode = std::ios::out;
        }
        else if (openMode == Permissions::ReadWrite)
        {
            mode = std::ios::out;
            if (filePath.Exists())
                mode |= std::ios::in;
        }

		if (openAsBinary)
			mode |= std::ios::binary;

        if ((!filePath.Exists() && (mode & openMode == Permissions::ReadOnly) != 0) || filePath.IsDirectory())
        {
            CE_LOG(Error, All, "FileStream constructed with a path that either does not exist or is a directory: {}", filePath);
            return;
        }

        impl = std::fstream((fs::path)filePath, mode);
    }

    FileStream::~FileStream()
    {
        if (impl.is_open())
            impl.close();
    }

    void FileStream::Write(const void* inData, u64 length)
    {
        impl.write(static_cast<const char*>(inData), length);
        offset += length;
    }

    s64 FileStream::Read(void* outData, u64 length)
    {
        impl.read(static_cast<char*>(outData), length);
        offset += length;
        return length;
    }

    u64 FileStream::GetCurrentPosition()
    {
        return offset;
    }

    void FileStream::Seek(s64 seekPos, SeekMode seekMode)
    {
        switch (seekMode)
        {
        case SeekMode::Begin: offset = seekPos; break;
        case SeekMode::Current: offset += seekPos; break;
        case SeekMode::End: offset = GetLength() - 1 + seekPos; break;
        default: ;
        }
        if (impl.eof())
            impl.clear();
        impl.seekg(offset, std::ios::beg);
    }

    bool FileStream::IsOpen()
    {
        return impl.is_open();
    }

    void FileStream::Close()
    {
        impl.close();
    }

    u64 FileStream::GetLength()
    {
        impl.seekg(0, std::ios::end);
        auto length = impl.tellg();
        impl.seekg(offset, std::ios::beg);
		if (length < 0)
		{
			length = 0;
		}
        return length;
    }

    u64 FileStream::GetCapacity()
    {
        return 0;
    }

    bool FileStream::IsOutOfBounds()
    {
        return impl.eof() || GetCurrentPosition() >= GetLength();
    }

    void FileStream::SetOutOfBounds()
    {
        offset = 0;
        offset = GetLength();
        impl.seekg(0, std::ios::end);
		Stream::Read();
    }
    
} // namespace CE

