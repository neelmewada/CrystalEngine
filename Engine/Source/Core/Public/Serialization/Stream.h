#pragma once

#include "CoreTypes.h"

namespace CE
{

    class Object;
    class TypeInfo;
    class ClassType;
    class StructType;

    enum class SeekMode
    {
        Begin = 0,
        Current = 1,
        End = 2
    };

	class StreamOutOfBoundException : public Exception
	{
	public:
		StreamOutOfBoundException(const String& message) : Exception(message)
		{}

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

        virtual Stream& operator++()
        {
            Seek(1, SeekMode::Current);
            return *this;
        }

        virtual Stream& operator--()
        {
            Seek(-1, SeekMode::Current);
            return *this;
        }

        virtual Stream& operator<<(const String& string);
        virtual Stream& operator>>(String& string);

        virtual Stream& operator<<(const Name& name);
        virtual Stream& operator>>(Name& name);

        virtual Stream& operator<<(const char* cString);

        virtual Stream& operator<<(const UUID& uuid);
        virtual Stream& operator>>(UUID& uuid);

		virtual Stream& operator<<(char ch);
		virtual Stream& operator>>(char& ch);

        virtual Stream& operator<<(u8 byte);
        virtual Stream& operator>>(u8& byte);

		virtual Stream& operator<<(bool boolean);
		virtual Stream& operator>>(bool& boolean);
        
        virtual Stream& operator<<(u16 integer);
        virtual Stream& operator>>(u16& integer);
        
        virtual Stream& operator<<(u32 integer);
        virtual Stream& operator>>(u32& integer);
        
        virtual Stream& operator<<(u64 integer);
        virtual Stream& operator>>(u64& integer);

        virtual Stream& operator<<(s8 integer);
        virtual Stream& operator>>(s8& integer);
        
        virtual Stream& operator<<(s16 integer);
        virtual Stream& operator>>(s16& integer);
        
        virtual Stream& operator<<(s32 integer);
        virtual Stream& operator>>(s32& integer);
        
        virtual Stream& operator<<(s64 integer);
        virtual Stream& operator>>(s64& integer);

        virtual Stream& operator<<(f32 single);
        virtual Stream& operator>>(f32& single);

		virtual Stream& operator<<(f64 decimal);
		virtual Stream& operator>>(f64& decimal);

	private:

		template<typename TInt> requires TIsNumericType<TInt>::Value
		void ReadNumberFromAscii(TInt& out)
		{
			String readStr = String(30);
			bool isFloat = false;

			auto cur = Read();
			if (cur == '-')
			{
				readStr.Append(cur);
				cur = Read();
			}
			if (cur == '+')
				cur = Read();
			if (cur == '.')
			{
				isFloat = true;
				readStr.Append(cur);
				cur = Read();
			}

			while (cur != '\r' && cur != '\n' && cur != '\0' && (String::IsNumeric(cur) || cur == '.'))
			{
				if (cur == '.')
					isFloat = true;
				readStr.Append(cur);
				cur = Read();
			}

			if (cur == '\r')
			{
				cur = Read();
			}

			f32 floatValue = 0;
			s64 intValue = 0;
			if (isFloat && String::TryParseFloat(readStr, floatValue))
			{
				out = static_cast<TInt>(floatValue);
			}
			else if (!isFloat && String::TryParseInteger(readStr, intValue))
			{
				out = static_cast<TInt>(intValue);
			}
		}

    public:

        void Write(const String& string)
        {
            *this << string;
        }
        
        virtual void Write(const void* inData, u64 length) = 0;
        
        virtual void Write(u8 inByte)
        {
            Write(&inByte, 1);
        }
        
        virtual s64 Read(void* outData, u64 length) = 0;
        
        virtual u8 Read()
        {
            u8 byte = 0;
            Read(&byte, 1);
            return byte;
        }

		virtual u8 ReadByte() { return Stream::Read(); }

		template<typename T> requires TIsIntegralType<T>::Value
		T ReadInteger()
		{
			T value = 0;
			Read(&value, sizeof(value));
			return value;
		}

        virtual void* GetRawDataPtr() const
        {
            return nullptr;
        }

        virtual u64 GetCurrentPosition() = 0;

        virtual void Seek(s64 seekPos, SeekMode seekMode = SeekMode::Begin) = 0;

        virtual bool IsOutOfBounds()
        {
            return GetCurrentPosition() >= GetCapacity();
        }

        virtual void SetOutOfBounds() = 0;

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
        virtual u64 GetCapacity() = 0;

        virtual bool CanResize() const { return false; }

        virtual void SetAutoResizeIncrement(u32 increment) {}

        /**
         * \brief Returns false if there is no hard limit to the size of this stream, ex: A file output stream.
         * True if there is a hard limit to the size of this stream, ex: A memory stream.
         */
        virtual bool HasHardSizeLimit() { return false; }

        virtual bool IsBinaryMode() const
        {
            return isBinaryMode;
        }

        virtual void SetBinaryMode(bool setBinaryMode)
        {
            isBinaryMode = setBinaryMode;
        }

        virtual bool IsAsciiMode() const
        {
            return !IsBinaryMode();
        }

        virtual void SetAsciiMode(bool setAsciiMode)
        {
            SetBinaryMode(!setAsciiMode);
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

        /*template<typename T>
        Stream& SerializeInByteOrder(T& value)
        {
            static_assert(!TIsSameType<T, u8>::Value, "Function SerializeInOrder should not be used for 8 bit integers");
            static_assert(!TIsSameType<T, s8>::Value, "Function SerializeInOrder should not be used for 8 bit integers");

            if (!RequiresByteSwapping())
            {
                SerializeBinary(&value, sizeof(T));
                return *this;
            }
            
            return SerializeInSwappedByteOrder(&value, sizeof(T));
        }*/

        //Stream& SerializeInSwappedByteOrder(void* value, u32 length);

        void SwapBytes(void* value, u32 length);

    protected:
        // State Variables

        b8 forceByteSwapping = false;

        b8 isBinaryMode = false;
    };

    ENUM_CLASS_FLAGS(Stream::Permissions);
    
} // namespace CE
