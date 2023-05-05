#pragma once

namespace CE
{

    /*class CORE_API FileStream : public Stream
    {
    public:
        FileStream(const IO::Path& path, Permissions openMode, bool isBinary = false);
        virtual ~FileStream();
        
        bool IsOpen() override;
        void Close() override;
        bool CanRead() override;
        bool CanWrite() override;
        bool CanResize() override;
        bool HasHardSizeLimit() override;
        
        void Serialize(void* value, u64 length) override;
        u64 GetLength() override;
        u64 GetMaximumSize() override;

        void Seek(u32 seekPos, SeekMode dir = SeekMode::Begin);

    private:
        Permissions openMode = Permissions::None;
        IO::Path filePath;
        std::fstream impl;
    };
    */
    
} // namespace CE
