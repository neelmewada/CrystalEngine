#pragma once

#include "Stream.h"

namespace CE
{

    class CORE_API BinaryStream : public Stream
    {
    public:
        
        Stream& operator<<(const String& string) override;
        Stream& operator>>(String& string) override;

        virtual Stream& operator<<(const u8& integer);
        virtual Stream& operator>>(u8& integer);
        
        virtual Stream& operator<<(const u16& integer);
        virtual Stream& operator>>(u16& integer);
        
        virtual Stream& operator<<(const u32& integer);
        virtual Stream& operator>>(u32& integer);
        
        virtual Stream& operator<<(const u64& integer);
        virtual Stream& operator>>(u64& integer);

        virtual Stream& operator<<(const s8& integer);
        virtual Stream& operator>>(s8& integer);
        
        virtual Stream& operator<<(const s16& integer);
        virtual Stream& operator>>(s16& integer);
        
        virtual Stream& operator<<(const s32& integer);
        virtual Stream& operator>>(s32& integer);
        
        virtual Stream& operator<<(const s64& integer);
        virtual Stream& operator>>(s64& integer);

        bool IsAsciiStream() const override
        {
            return false;
        }
        
        bool IsBinaryStream() const override
        {
            return true;
        }
    };
    
} // namespace CE
