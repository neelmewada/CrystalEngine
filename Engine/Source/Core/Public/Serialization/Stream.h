#pragma once

#include "CoreTypes.h"

namespace CE
{

    class Object;

    enum class SeekMode
    {
        Begin = 0,
        Current = 1,
        End = 2
    };

    /*
     *  Base class for all types of streams used to load and save different type of data.
     *  Ex: memory stream, file stream, structured stream, etc
     */
    class CORE_API Stream
    {
    public:
        enum class Permissions
        {
            None = 0,
            ReadOnly = 1,
            WriteOnly = 2,
            ReadWrite = 3,
        };
        
        Stream() = default;
        Stream(const Stream&) = default;
        Stream& operator=(const Stream&) = default;
        virtual ~Stream() = default;

        virtual Stream& operator<<(u8& value)
        {
            Serialize(value);
            return *this;
        }

        virtual Stream& operator<<(u16& value);

        virtual Stream& operator<<(String& string)
        {
            Serialize(string);
            return *this;
        }

        virtual Stream& operator<<(Name& name)
        {
            auto str = name.GetString();
            return *this << str;
        }

    public:
        /**
         * \brief Low level function that reads or writes raw byte data to/from the stream depending on the mode
         */
        virtual void Serialize(void* value, u64 length) = 0;

        virtual void Serialize(u8& byte) = 0;

        virtual void Serialize(String& string) = 0;

        virtual void* GetRawDataPtr() const
        {
            return nullptr;
        }

        //////////////////////////////////////////////////////////////
        // State & Traits

        /**
         * \brief Returns true if the stream is open to be used. False if it failed to open.
         */
        virtual bool IsOpen() = 0;

        virtual void Close() = 0;

        /**
         * \brief Some stream types do not support reading
         */
        virtual bool CanRead() { return false; }

        /**
         * \brief Some stream types do not support writing
         */
        virtual bool CanWrite() { return true; }

        /**
         * \brief Returns the length/size of the actual meaningful data which is always less than or equal to the max size. 
         */
        virtual u64 GetLength() = 0;

        /**
         * \brief Returns the maximum amount of data that can be stored in this stream
         */
        virtual u64 GetMaximumSize() = 0;

        virtual bool CanResize() { return false; }

        /**
         * \brief Returns false if there is no hard limit to the size of this stream, ex: A file output stream.
         * True if there is a hard limit to the size of this stream, ex: A memory stream.
         */
        virtual bool HasHardSizeLimit() { return false; }

        virtual bool IsReading() { return isReading; }

        virtual void SetIsReading(b8 isReading)
        {
            this->isReading = isReading;
        }

        virtual bool IsWriting() { return !IsReading(); }

        virtual void SetIsWriting(b8 isWriting)
        {
            SetIsReading(!isWriting);
        }

        virtual bool IsBinarySerialization() { return isBinarySerialization; }

        virtual void SetAsBinarySerialization(b8 setAsBinary)
        {
            this->isBinarySerialization = true;
        }

        virtual bool IsASCIISerialization() { return !IsBinarySerialization(); }

        virtual void SetAsASCIISerialization(b8 setAsASCII)
        {
            SetAsBinarySerialization(!setAsASCII);
        }

        /**
         * \brief Returns true if bytes need to be swapped in case of little endian system
         */
        bool RequiresByteSwapping()
        {
#if PAL_TRAIT_LITTLE_ENDIAN
            return forceByteSwapping;
#elif PAL_TRAIT_BIG_ENDIAN
            return forceByteSwapping;
#endif
        }

    protected:

        virtual void ReadInteger(u16& out) = 0;
        virtual void ReadInteger(u32& out) = 0;
        virtual void ReadInteger(u64& out) = 0;

        template<typename T>
        Stream& SerializeInByteOrder(T& value)
        {
            static_assert(!TIsSameType<T, u8>::Value, "Function SerializeInOrder should not be used for 8 bit integers");
            static_assert(!TIsSameType<T, s8>::Value, "Function SerializeInOrder should not be used for 8 bit integers");

            if (!RequiresByteSwapping())
            {
                Serialize(&value, sizeof(T));
                return *this;
            }
            
            return SerializeInSwappedByteOrder(&value, sizeof(T));
        }

        Stream& SerializeInSwappedByteOrder(void* value, u32 length);

        void SwapBytes(void* value, u32 length);

    protected:
        // State Variables
        
        u32 nextReadCount = 0;

        b8 isReading = false;

        b8 forceByteSwapping = false;

        b8 isBinarySerialization = true;
    };

    ENUM_CLASS_FLAGS(Stream::Permissions);
    
} // namespace CE
