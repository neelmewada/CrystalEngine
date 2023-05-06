#pragma once

namespace CE
{
    
    class CORE_API FileStream : public Stream
    {
    public:
        FileStream(const IO::Path& filePath, Permissions openMode = Permissions::ReadWrite);

        virtual ~FileStream();

        // File streams cannot be copied
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;
        
        void Write(const void* inData, u64 length) override;
        
        void Read(void* outData, u64 length) override;
        
        u64 GetCurrentPosition() override;
        
        void Seek(s64 seekPos, SeekMode seekMode) override;
        
        bool IsOpen() override;
        
        void Close() override;
        
        u64 GetLength() override;
        
        u64 GetCapacity() override;

        bool IsOutOfBounds() override;

        void SetOutOfBounds() override;
        
    protected:
        IO::Path filePath;
        std::fstream impl;
        u32 offset = 0;

        Permissions openMode = Permissions::ReadOnly;
    };
    
} // namespace CE