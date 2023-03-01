
#include "CoreTypes.h"

using namespace CE;

FixedSizeFreeListAllocator<STRING_BUFFER_SIZE, STRING_BUFFER_GROW_COUNT> String::StaticBufferAllocator{ STRING_BUFFER_GROW_COUNT };

String::String()
{
	Reserve(STRING_BUFFER_SIZE - 1);
}

String::String(std::string string) : String(string.c_str())
{

}

String::String(StringView stringView) : String()
{
    Reserve(stringView.GetSize());
    CopyCString(stringView.GetCString(), stringView.GetSize());
}

String::String(u32 reservedSize)
{
	Reserve(reservedSize);
}

String::String(const char* value)
{
    if (value == nullptr)
    {
        Reserve(1);
        SetCString("");
        return;
    }

	Reserve(strlen(value));
	SetCString(value);
}

String::String(const char* cStringA, const char* cStringB)
{
	Reserve(strlen(cStringA) + strlen(cStringB));
	SetCString(cStringA);
	ConcatenateCString(cStringB);
}

String::String(Iterator begin, Iterator end)
{
    auto length = end.Ptr - begin.Ptr;
    Reserve(length);
    CopyCString(begin.Ptr, length);
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
    // High capacity values are often because of garbage values and can cause crashes because String buffer is pointing to garbage location
    if (Capacity > 16_MB)
    {
        // Reset string to defaults
        Capacity = 0;
        Buffer = nullptr;
    }

    reserveCharacterCount++; // Add extra byte for null terminator

    if (reserveCharacterCount > STRING_BUFFER_SIZE) // Use dynamic buffer
    {
        if (!bIsUsingDynamicBuffer && Buffer == nullptr) // If we need a dynamic buffer from the get-go
        {
            Capacity = reserveCharacterCount;
            Buffer = new char[Capacity];
        }
        else if (!bIsUsingDynamicBuffer && Buffer != nullptr) // If we were using static buffer earlier
        {
            auto oldBuffer = Buffer;

            Capacity = reserveCharacterCount;
            Buffer = new char[Capacity];
            if (StringLength > 0)
                memcpy(Buffer, oldBuffer, StringLength + 1);

            StaticBufferAllocator.Free(oldBuffer); // Release the previously used static buffer
        }
        else if (bIsUsingDynamicBuffer && reserveCharacterCount > Capacity && Buffer != nullptr) // If we were already using dynamic buffer, but need more capacity
        {
            auto stagingBuffer = new char[reserveCharacterCount];
            if (StringLength > 0)
                memcpy(stagingBuffer, Buffer, StringLength + 1);

            Capacity = reserveCharacterCount;
            delete[] Buffer;
            Buffer = stagingBuffer;
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

StringView CE::String::ToStringView() const
{
    return StringView(*this);
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
    Buffer[StringLength] = 0;
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
    memcpy(Buffer, cString, StringLength);
    Buffer[StringLength] = 0;
}

/*
 *  Helper/Utility
 */

void CE::String::Append(char c)
{
    char copyStr[] = { c, '\0' };
    ConcatenateCString(copyStr);
}

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

bool String::StartsWith(const String& string) const
{
    return StartsWith(string.GetCString());
}

bool String::StartsWith(const char* cString) const
{
    int charIndex = 0;
    if (cString == nullptr)
        return true;

    while (*cString != 0)
    {
        if (charIndex >= GetLength())
            return false;

        if (Buffer[charIndex] != *cString)
            return false;

        charIndex++;
        cString++;
    }

    return true;
}

bool CE::String::Contains(const String& string) const
{
    int thisIndex = 0;

    while (thisIndex < GetLength())
    {
        StringView stringView = StringView(Buffer + thisIndex, GetLength() - thisIndex);
        if (stringView.StartsWith(string.ToStringView()))
        {
            return true;
        }

        thisIndex++;
    }

    return false;
}

bool CE::String::Contains(const char* string) const
{
    int thisIndex = 0;
    StringView other = StringView(string);

    while (thisIndex < GetLength())
    {
        StringView stringView = StringView(Buffer + thisIndex, GetLength() - thisIndex);
        if (stringView.StartsWith(other))
        {
            return true;
        }

        thisIndex++;
    }

    return false;
}

bool CE::String::Search(const String& string) const
{
    int thisIndex = 0;
    String thisLower = ToLower();
    String otherLower = string.ToLower();

    return thisLower.Contains(otherLower);
}

bool CE::String::Search(const char* string) const
{
    int thisIndex = 0;
    String thisLower = ToLower();
    String otherLower = String(string).ToLower();

    return thisLower.Contains(otherLower);
}

String CE::String::ToLower() const
{
    String result{ GetLength() };

    for (int i = 0; i < GetLength(); i++)
    {
        char ch = Buffer[i];
        if (ch >= 'A' && ch <= 'Z')
            result[i] = std::tolower(ch);
        else
            result[i] = ch;
    }

    result.Buffer[GetLength()] = 0;
    result.StringLength = std::strlen(result.Buffer);

    return result;
}

String CE::String::ToUpper() const
{
    String result{ GetLength() };

    for (int i = 0; i < GetLength(); i++)
    {
        char ch = Buffer[i];
        if (ch >= 'a' && ch <= 'z')
            result[i] = std::toupper(ch);
        else
            result[i] = ch;
    }

    result.Buffer[GetLength()] = 0;
    result.StringLength = std::strlen(result.Buffer);

    return result;
}

String String::GetSubstring(int startIndex, int length)
{
    if (length == -1)
    {
        return String(Buffer + startIndex);
    }

    if (length == 0)
    {
        return "";
    }

    return String(Iterator(Begin().Ptr + startIndex), Iterator(Begin().Ptr + startIndex + length));
}

StringView CE::String::GetSubstringView(int startIndex, int length) const
{
    if (length == -1)
    {
        return StringView(Buffer + startIndex);
    }

    if (length == 0)
    {
        return StringView();
    }

    return StringView(GetCString() + startIndex, length);
}

Array<String> String::Split(char delimiter)
{
    int startIdx = 0;
    int endIdx = 0;

    Array<String> result{};

    for (int i = 0; i < StringLength; i++)
    {
        if (Buffer[i] == delimiter && startIdx < StringLength)
        {
            result.Add(GetSubstringView(startIdx, endIdx - startIdx)); // Don't add +1: we don't want the delimiter present in the split string

            startIdx = i + 1;
            endIdx = startIdx;
            continue;
        }
        else if (i == StringLength - 1) // last character
        {
            result.Add(GetSubstringView(startIdx, endIdx - startIdx + 1)); // +1 to include the last character
            continue;
        }

        endIdx++;
    }

    return result;
}

String CE::String::RemoveWhitespaces()
{
    String result{ GetLength() };
    int index = 0;

    for (int i = 0; i < GetLength(); i++)
    {
        if (Buffer[i] != ' ')
        {
            result[index++] = Buffer[i];
        }
    }

    result[index++] = 0;
    result.StringLength = strlen(result.Buffer);

    return result;
}


