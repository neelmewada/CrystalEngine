
#include "CoreMinimal.h"

namespace CE
{

    Stream& BinaryStream::operator<<(const String& string)
    {
        Write(string.GetCString(), string.GetLength());
        Write('\0'); // null terminator
        return *this;
    }

    Stream& BinaryStream::operator>>(String& string)
    {
        auto startPos = GetCurrentPosition();
        
        while (Read() != 0) {} // Read bytes until we encounter a null terminator
        
        if (startPos < GetCurrentPosition())
        {
            u32 count = GetCurrentPosition() - startPos; // last will always be null terminator
            string.Reserve(count);
            Seek(startPos, SeekMode::Begin);
            Read(string.GetCString(), count);
        }
        
        return *this;
    }

    Stream& BinaryStream::operator<<(const u8& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(u8& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const u16& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(u16& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const u32& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(u32& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const u64& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(u64& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const s8& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(s8& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const s16& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(s16& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const s32& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(s32& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator<<(const s64& integer)
    {
        Write(&integer, sizeof(integer));
        return *this;
    }

    Stream& BinaryStream::operator>>(s64& integer)
    {
        Read(&integer, sizeof(integer));
        return *this;
    }
    
} // namespace CE

