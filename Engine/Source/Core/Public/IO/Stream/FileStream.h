#pragma once

#include "GenericStream.h"

#include <fstream>

namespace CE::IO
{

    class CORE_API FileStream : public GenericStream
    {
    public:

        FileStream()
            : fStream()
        {}

        FileStream(Path filePath, OpenMode fileMode)
            : fileMode(fileMode), filePath(filePath)
        {
            if (filePath.IsDirectory())
            {
                valid = false;
                return;
            }
            Path baseDir = filePath.GetParentPath();
            if (!baseDir.Exists())
            {
                Path::CreateDirectories(baseDir);
            }
            fStream = std::fstream(filePath.Impl, (std::ios_base::openmode)fileMode);
            valid = true;
        }

        virtual bool        IsValid() const { return valid; }
        virtual bool        IsOpen() const override;
        virtual bool        CanSeek() const override;
        virtual bool        CanRead() const override;
        virtual bool        CanWrite() const override;
        virtual void        Seek(SSIZE_T offsetInBytes, SeekMode mode) override;
        virtual SIZE_T      Read(SIZE_T bytes, void* outBuffer) override;
        virtual u8          ReadNextByte() override;
        virtual SIZE_T      Write(SIZE_T bytes, const void* inBuffer) override;

        virtual const char* GetRawPointer() const override;

        virtual SIZE_T      GetCurPos() const override;

        virtual SIZE_T      GetLength() const override;

        virtual String      GetFilename() const override { return filePath.GetString(); }
        virtual OpenMode    GetModeFlags() const override { return fileMode; }
        virtual bool        ReOpen() override;
        virtual void        Close() override;

    private:
        bool valid = true;
        SIZE_T currentOffset = 0;
        SeekMode seekMode = ST_SEEK_BEGIN;

        OpenMode fileMode = OpenMode::ModeRead;
        Path filePath;

        std::fstream fStream;
    };
    
} // namespace CE::IO
