
#include "Containers/String.h"
#include "Misc/CoreLiterals.h"

using namespace CE;

FixedSizeFreeListAllocator<STRING_BUFFER_SIZE, STRING_BUFFER_GROW_COUNT> String::StaticBufferAllocator{ STRING_BUFFER_GROW_COUNT };

String::String()
{
	Reserve(STRING_BUFFER_SIZE - 1);
}

String::String(std::string string) : String(string.c_str())
{

}

String::String(u32 reservedSize)
{
	Reserve(reservedSize);
}

String::String(const char* value)
{
	Reserve(strlen(value));
	SetCString(value);
}

String::String(const char* cStringA, const char* cStringB)
{
	Reserve(strlen(cStringA) + strlen(cStringB));
	SetCString(cStringA);
	ConcatenateCString(cStringB);
}

String::String(String&& move) noexcept
	: bIsUsingDynamicBuffer(move.bIsUsingDynamicBuffer),
	StringLength(move.StringLength),
	Capacity(move.Capacity),
	Buffer(move.Buffer)
{
	// Zero out Move
	move.Buffer = nullptr;
	move.Capacity = 0;
	move.StringLength = 0;
	move.bIsUsingDynamicBuffer = false;
}

String::String(const String& copy)
{
	CopyCString(copy.GetCString(), copy.StringLength);
}

String& String::operator=(const String& rhs)
{
	if (&rhs != this)
	{
		CopyCString(rhs.GetCString(), rhs.StringLength);
	}
	return *this;
}

String& String::operator=(const std::string& rhs)
{
	Reserve(rhs.length());
	SetCString(rhs.c_str());
	return *this;
}

String& String::operator=(const char* cString)
{
	SetCString(cString);
	return *this;
}

String::~String()
{
	if (!bIsUsingDynamicBuffer && Buffer != nullptr)
	{
		StaticBufferAllocator.Free(Buffer);
	}
	else
	{
		delete Buffer;
	}
}

void String::Reserve(u32 reserveCharacterCount)
{
    if (Capacity > 16_MB) // High capacity values are often because of garbage values and can cause crashes because it's higher than reserveCharacterCount.
    {
        Capacity = 0;
    }

    reserveCharacterCount++; // Add extra byte for null terminator

    if (reserveCharacterCount > 16_MB)
    {
        //LOG_ERROR("Error: FString::Reserve() called with a capacity higher than 16 MB! Value: %i", reserveCharacterCount);
    }

    if (reserveCharacterCount > STRING_BUFFER_SIZE) // Use dynamic buffer
    {
        if (!bIsUsingDynamicBuffer && Buffer == nullptr) // If we need a dynamic buffer from the get-go
        {
            Capacity = reserveCharacterCount;
            Buffer = new char[Capacity];
        }
        else if (!bIsUsingDynamicBuffer && Buffer != nullptr) // If we were using static buffer earlier
        {
            auto OldBuffer = Buffer;

            Capacity = reserveCharacterCount;
            Buffer = new char[Capacity];
            if (StringLength > 0)
                memcpy(Buffer, OldBuffer, StringLength + 1);

            StaticBufferAllocator.Free(OldBuffer); // Release the previously used static buffer
        }
        else if (bIsUsingDynamicBuffer && reserveCharacterCount > Capacity) // If we were already using dynamic buffer, but need more capacity
        {
            auto StagingBuffer = new char[reserveCharacterCount];
            if (StringLength > 0)
                memcpy(StagingBuffer, Buffer, StringLength + 1);

            Capacity = reserveCharacterCount;
            delete[] Buffer;
            Buffer = StagingBuffer;
        }

        bIsUsingDynamicBuffer = true;
    }
    else if (!bIsUsingDynamicBuffer && Buffer == nullptr) // If static buffer is enough
    {
        Capacity = reserveCharacterCount;
        Buffer = (char*)StaticBufferAllocator.Allocate();
    }
}

const char* String::GetCString() const
{
    return Buffer;
}

void String::SetCString(const char* cString)
{
    if (cString == nullptr) // Clear the string
    {
        if (Capacity > 0) Buffer[0] = 0;
        StringLength = 0;
        return;
    }

    StringLength = strlen(cString);

    if (StringLength == 0) // Clear the string
    {
        if (Capacity > 0) Buffer[0] = 0;
        return;
    }

    Reserve(StringLength);
    memcpy(Buffer, cString, StringLength + 1);
}

void String::CopyCString(const char* cString, u32 copyStringLength)
{
    Reserve(copyStringLength);

    if (cString == nullptr) // Clear the string
    {
        if (Capacity > 0) Buffer[0] = 0;
        return;
    }

    StringLength = copyStringLength;

    if (StringLength == 0) // Clear the string
    {
        if (Capacity > 0) Buffer[0] = 0;
        return;
    }

    Reserve(StringLength);
    memcpy(Buffer, cString, StringLength + 1);
}

/*
 *  Helper/Utility
 */

void String::Concatenate(const String& string)
{
    ConcatenateCString(string.GetCString());
}

void String::ConcatenateCString(const char* cString)
{
    auto cStringLength = strlen(cString);
    Reserve(StringLength + cStringLength);
    memcpy(Buffer + StringLength, cString, cStringLength + 1);
    StringLength += cStringLength;
}

void String::Concatenate(s64 integer)
{
    Concatenate(std::to_string(integer));
}

