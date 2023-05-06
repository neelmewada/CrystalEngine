#pragma once

namespace CE
{

    namespace Internal
    {
        class CORE_API FileStreamBase
        {
        protected:
            IO::Path filePath;
            std::fstream impl;

            Stream::Permissions openMode = Stream::Permissions::ReadOnly;
        };
    }

    template<class TBase>
    class FileStreamBase : public TBase
    {
    public:
        FileStreamBase(const IO::Path& filePath, Stream::Permissions openMode = Stream::Permissions::ReadWrite)
        {
            ASSERT(openMode != Stream::Permissions::None, "FileAsciiStream constructed with openMode as None!")

            this->filePath = filePath;

            this->openMode = openMode;
            std::ios::openmode mode = 0;
            if (openMode == Stream::Permissions::ReadOnly)
            {
                mode = std::ios::in;
            }
            else if (openMode == Stream::Permissions::WriteOnly)
            {
                mode = std::ios::out;
            }
            else if (openMode == Stream::Permissions::ReadWrite)
            {
                mode = std::ios::out;
                if (filePath.Exists())
                    mode |= std::ios::in;
            }

            mode |= std::ios::binary;

            if ((!filePath.Exists() && (mode & openMode == Stream::Permissions::ReadOnly) != 0) || filePath.IsDirectory())
            {
                CE_LOG(Error, All, "FileStream constructed with a path that either does not exist or is a directory: {}", filePath);
                return;
            }

            impl = std::fstream((fs::path)filePath, mode);
        }

        virtual ~FileStreamBase()
        {
            if (impl.is_open())
                impl.close();
        }
        
        void Write(const void* inData, u64 length) override
        {
            impl.write(static_cast<const char*>(inData), length);
        }
        
        void Read(void* outData, u64 length) override
        {
            impl.read(static_cast<char*>(outData), length);
        }
        
        u64 GetCurrentPosition() override
        {
            return impl.tellp();
        }
        
        void Seek(s64 seekPos, SeekMode seekMode) override
        {
            switch (seekMode)
            {
            case SeekMode::Begin: impl.seekp(seekPos, std::ios::beg); break;
            case SeekMode::Current: impl.seekp(seekPos, std::ios::cur); break;
            case SeekMode::End: impl.seekp(seekPos, std::ios::end); break;
            }
        }
        
        bool IsOpen() override
        {
            return impl.is_open();
        }
        
        void Close() override
        {
            impl.close();
        }
        
        u64 GetLength() override
        {
            return impl.tellg();
        }
        
        u64 GetCapacity() override
        {
            return GetLength();
        }
        
    protected:
        IO::Path filePath;
        std::fstream impl;

        Stream::Permissions openMode = Stream::Permissions::ReadOnly;
    };

    using FileAsciiStream = FileStreamBase<AsciiStream>;
    using FileBinaryStream = FileStreamBase<BinaryStream>;
    
} // namespace CE
