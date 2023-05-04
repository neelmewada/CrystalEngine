#pragma once

#include "CoreTypes.h"

namespace CE
{

    class Object;

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
        ENUM_CLASS_FLAGS(Permissions);
        
        Stream() = default;
        Stream(const Stream&) = default;
        Stream& operator=(const Stream&) = default;
        virtual ~Stream() = default;

        virtual Stream& operator<<(u8& value)
        {
            Serialize(&value, 1);
            return *this;
        }

        virtual Stream& operator<<(u16& value)
        {
            SerializeInByteOrder(value);
            return *this;
        }

        virtual Stream& operator<<(u32& value)
        {
            SerializeInByteOrder(value);
            return *this;
        }

        virtual Stream& operator<<(u64& value)
        {
            SerializeInByteOrder(value);
            return *this;
        }

        virtual Stream& operator<<(String& string);
        virtual Stream& operator<<(char& ch);

        virtual Stream& operator<<(UUID& uuid);
        
        virtual Stream& operator<<(Name& name);

        virtual Stream& operator<<(Object*& object)
        {
            return *this;
        }

    public:
        /**
         * \brief Low level function that reads or writes raw byte data into/from the stream depending on the mode
         */
        virtual void Serialize(void* value, u64 length) = 0;

        FORCE_INLINE Stream& SerializeInByteOrder(void* value, u32 length)
        {
            if (!RequiresByteSwapping())
            {
                Serialize(value, length);
                return *this;
            }
            SerializeInSwappedByteOrder(value, length);
        }

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

        virtual bool IsLoading() { return isLoading; }

        virtual void SetIsLoading(b8 isLoading)
        {
            this->isLoading = isLoading;
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

        b8 isLoading = false;

        b8 forceByteSwapping = false;
    };
    
} // namespace CE
