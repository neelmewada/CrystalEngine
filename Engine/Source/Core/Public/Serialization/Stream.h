#pragma once

#include "CoreTypes.h"

namespace CE
{

    /*
     *  Base class for all types of streams used to load and save different type of data.
     *  Ex: memory stream, file stream, structured stream, etc
     */
    class CORE_API Stream
    {
    public:
        Stream() = default;
        Stream(const Stream&) = default;
        Stream& operator=(const Stream&) = default;
        ~Stream() = default;

        virtual Stream& operator<<(u8& value)
        {
            return *this;
        }

        virtual Stream& operator<<(u16& value)
        {
            return *this;
        }

        virtual Stream& operator<<(u32& value)
        {
            return *this;
        }

        virtual Stream& operator<<(u64& value)
        {
            return *this;
        }

        virtual Stream& operator<<(String& name);
        virtual Stream& operator<<(char& ch);

    public:
        /**
         * \brief Low level function that serializes raw byte data into the stream.
         */
        virtual void Serialize(const void* value, u64 length) = 0;

        virtual void Serialize(u8 byte)
        {
            Serialize(&byte, 1);
        }

        virtual void Read(void* outValue, u64 length) = 0;

        virtual void ReadNextByte(u8& outByte)
        {
            Read(&outByte, 1);
        }

        /**
         * \brief Returns true if the stream is open to be used. False if it failed to open.
         */
        virtual bool IsOpen() = 0;

        /**
         * \brief Some stream types do not support reading
         */
        virtual bool CanRead() { return false; }

        /**
         * \brief Some stream types do not support writing
         */
        virtual bool CanWrite() { return true; }

        //////////////////////////////////////////////////////////////
        // Getters & Setters

        /**
         * \brief Returns the length/size of the actual meaningful data which is always less than or equal to the max size. 
         */
        virtual u64 GetLength() = 0;

        /**
         * \brief Returns the maximum amount of data that can be stored in this stream
         */
        virtual u64 GetMaximumSize() = 0;

        virtual bool CanResize() { return false; }

        virtual bool IsLoading() { return true; }

        virtual void SetIsLoading(b8 isLoading) {}

        /**
         * \brief Returns false if there is no hard limit to the size of this stream, ex: A file output stream.
         * True if there is a hard limit to the size of this stream, ex: A memory stream.
         */
        virtual bool HasHardSizeLimit() { return false; }

    protected:
        
        u32 nextReadCount = 0;
    };
    
} // namespace CE
