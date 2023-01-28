
#include "IO/Stream/FileStream.h"


namespace CE::IO
{

	bool FileStream::IsOpen() const
	{
		return fStream.is_open();
	}

    bool FileStream::CanSeek() const
    {
        return true;
    }

    bool FileStream::CanRead() const
    {
        return (u32)(fileMode & OpenMode::ModeRead);
    }

    bool FileStream::CanWrite() const
    {
        return (u32)(fileMode & OpenMode::ModeWrite) || (u32)(fileMode & OpenMode::ModeAppend) || (u32)(fileMode & OpenMode::ModeBinary);
    }

    void FileStream::Seek(SSIZE_T offsetInBytes, SeekMode mode)
    {
        seekMode = mode;
        currentOffset = ComputeSeekPosition(offsetInBytes, mode);

        if (CanRead())
        {
            fStream.seekg(currentOffset, std::ios_base::beg);
        }

        if (CanWrite())
        {
            fStream.seekp(currentOffset, std::ios_base::beg);
        }
    }

    SIZE_T FileStream::Read(SIZE_T bytes, void* outBuffer)
    {
        fStream.read((char*)outBuffer, bytes);
        return fStream.gcount();
    }

    u8 FileStream::ReadNextByte()
    {
        SSIZE_T curPos = GetCurPos();
        if (seekMode == ST_SEEK_BEGIN || seekMode == ST_SEEK_CUR)
        {
            curPos = curPos + 1;
        }
        else if (seekMode == ST_SEEK_END)
        {
            curPos = curPos - GetLength() - 1;
        }

        char c = 0;
        fStream.read(&c, 1);
        Seek(curPos, seekMode);
        return static_cast<u8>(c);
    }

    SIZE_T FileStream::Write(SIZE_T bytes, const void* inBuffer)
    {
        SSIZE_T curPos = GetCurPos();
        if (seekMode == ST_SEEK_BEGIN || seekMode == ST_SEEK_CUR)
        {
            curPos = curPos + bytes;
        }
        else if (seekMode == ST_SEEK_END)
        {
            curPos = curPos - GetLength() - bytes;
        }

        curPos = GetCurPos();
        fStream.write((const char*)inBuffer, bytes);
        SIZE_T count = fStream.gcount();
        Seek(curPos, seekMode);
        return count;
    }

    const char* FileStream::GetRawPointer() const
    {
        return nullptr;
    }

    SIZE_T FileStream::GetCurPos() const
    {
        return currentOffset;
    }

    SIZE_T FileStream::GetLength() const
    {
        return std::filesystem::file_size(filePath.Impl);
    }

    bool FileStream::ReOpen()
    {
        if (fStream.is_open())
            fStream.close();
        fStream.open(filePath.Impl, (std::ios_base::openmode)fileMode);
        return fStream.is_open();
    }

    void FileStream::Close()
    {
        fStream.close();
    }

} // namespace CE::IO

