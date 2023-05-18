
#include <regex>

#include "CoreMinimal.h"

using namespace CE;

namespace CE
{
    static FixedSizeFreeListAllocator<STRING_BUFFER_SIZE, STRING_BUFFER_GROW_COUNT> StaticBufferAllocator{ STRING_BUFFER_GROW_COUNT };

    static Mutex stringAllocMutex{};

    static ThreadId gMainThreadId = Thread::GetCurrentThreadId();

    namespace Internal
    {
        class CORE_API StaticStringBlockAllocator : public ThreadSingleton<StaticStringBlockAllocator>
        {
        public:
            StaticStringBlockAllocator() {}
            virtual ~StaticStringBlockAllocator()
            {
                if (Thread::GetCurrentThreadId() == gMainThreadId)
                {
                    // Do NOT clean up string buffers on main thread.
                    // Sometimes, static/global destructors are be called after thread singleton is destroyed, which can cause crash on app exit
                    return;
                }
                staticBufferAllocator.FreeAll();
            }
        
            FixedSizeFreeListAllocator<STRING_BUFFER_SIZE, STRING_BUFFER_GROW_COUNT> staticBufferAllocator{ STRING_BUFFER_GROW_COUNT };
        };
    }

    String::String() : threadId(Thread::GetCurrentThreadId())
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

    String::String(u32 reservedSize) : threadId(Thread::GetCurrentThreadId())
    {
        Reserve(reservedSize);
    }

    String::String(const char* value) : threadId(Thread::GetCurrentThreadId())
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

    String::String(const char* cStringA, const char* cStringB) : threadId(Thread::GetCurrentThreadId())
    {
        Reserve(strlen(cStringA) + strlen(cStringB));
        SetCString(cStringA);
        ConcatenateCString(cStringB);
    }

    String::String(Iterator begin, Iterator end) : threadId(Thread::GetCurrentThreadId())
    {
        auto length = end.Ptr - begin.Ptr;
        Reserve(length);
        CopyCString(begin.Ptr, length);
    }

    String::String(String&& move) noexcept
        : Buffer(move.Buffer)
        , Capacity(move.Capacity)
        , bIsUsingDynamicBuffer(move.bIsUsingDynamicBuffer)
        , StringLength(move.StringLength)
        , threadId(Thread::GetCurrentThreadId())
    {
        // Zero out Move
        move.Buffer = nullptr;
        move.Capacity = 0;
        move.StringLength = 0;
        move.bIsUsingDynamicBuffer = false;
        move.threadId = 0;
    }

    String::String(const String& copy) : threadId(Thread::GetCurrentThreadId())
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
            Internal::StaticStringBlockAllocator::Get().staticBufferAllocator.Free(Buffer);
            //StaticBufferAllocator.Free(Buffer);
        }
        else
        {
            delete[] Buffer;
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
			return;
        }

        reserveCharacterCount++; // Add extra byte for null terminator

        if (reserveCharacterCount > STRING_BUFFER_SIZE) // Use dynamic buffer
        {
            if (!bIsUsingDynamicBuffer && Buffer == nullptr) // If we need a dynamic buffer from the get-go
            {
                Capacity = reserveCharacterCount;
                Buffer = new char[Capacity];
                Buffer[0] = 0;
            }
            else if (!bIsUsingDynamicBuffer && Buffer != nullptr) // If we were using static buffer earlier
            {
                auto staticBuffer = Buffer;

                Capacity = reserveCharacterCount;
                Buffer = new char[Capacity];
                if (StringLength > 0)
                    memcpy(Buffer, staticBuffer, StringLength + 1);
                else
                    Buffer[0] = 0;

                Internal::StaticStringBlockAllocator::Get().staticBufferAllocator.Free(staticBuffer);
            }
            else if (bIsUsingDynamicBuffer && reserveCharacterCount > Capacity && Buffer != nullptr) // If we were already using dynamic buffer, but need more capacity
            {
                auto stagingBuffer = new char[reserveCharacterCount];
                if (StringLength > 0)
                    memcpy(stagingBuffer, Buffer, StringLength + 1);
                else
                    Buffer[0] = 0;

                Capacity = reserveCharacterCount;
                delete[] Buffer;
                Buffer = stagingBuffer;
            }

            bIsUsingDynamicBuffer = true;
        }
        else if (!bIsUsingDynamicBuffer && Buffer == nullptr) // If static buffer is enough
        {
            Capacity = reserveCharacterCount;
            Buffer = (char*)Internal::StaticStringBlockAllocator::Get().staticBufferAllocator.Allocate();
            //Buffer = (char*)StaticBufferAllocator.Allocate();
            Buffer[0] = 0;
            bIsUsingDynamicBuffer = false;
        }
    }

    char* String::GetCString() const
    {
        return Buffer;
    }

	char* String::GetData() const
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
        Reserve(copyStringLength); // reserve: copyStringLength + 1

        if (cString == nullptr) // Clear the string
        {
            if (Capacity > 0 && Buffer != nullptr) Buffer[0] = 0;
            return;
        }

        StringLength = copyStringLength;

        if (StringLength == 0) // Clear the string
        {
            if (Capacity > 0 && Buffer != nullptr) 
                Buffer[0] = 0;
            return;
        }

        Reserve(StringLength);
		if (Buffer != nullptr)
		{
			memcpy(Buffer, cString, StringLength);
			Buffer[StringLength] = 0;
		}
    }

    /*
     *  Helper/Utility
     */

    bool String::RegexMatch(const String& sourceString, const String& regex)
    {
        return std::regex_match(sourceString.GetCString(), std::regex(regex.GetCString()));
    }

    bool String::RegexMatch(const String& sourceString, const char* regex)
    {
        return std::regex_match(sourceString.GetCString(), std::regex(regex));
    }

    void String::Append(char c)
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

    bool String::EndsWith(const String& string) const
    {
        int thisIdx = GetLength() - 1;
        int otherIdx = string.GetLength() - 1;

        if (GetLength() < string.GetLength())
            return false;

        while (thisIdx >= 0 && otherIdx >= 0)
        {
            if (Buffer[thisIdx] != string[otherIdx])
                return false;

            thisIdx--;
            otherIdx--;
        }

        return true;
    }

    bool String::EndsWith(const char* cString) const
    {
        return EndsWith(String(cString));
    }

    bool String::Contains(const String& string) const
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

    bool String::Contains(char character) const
    {
        for (int i = 0; i < GetLength(); i++)
        {
            if (Buffer[i] == character)
                return true;
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

    void CE::String::Split(String delimiter, Array<String>& outArray)
    {
        int startIdx = 0;

        for (int i = 0; i < StringLength; i++)
        {
            char ch = Buffer[i];

            StringView view = StringView(Buffer + i);

            bool isLast = (i == StringLength - 1);
            if (view.StartsWith(delimiter) || isLast)
            {
                if (isLast)
                    i++;

                if (startIdx != i)
                {
                    outArray.Add(GetSubstringView(startIdx, i - startIdx));
                }

                startIdx = i + delimiter.GetLength();
                i += delimiter.GetLength() - 1;
            }
        }
    }

    String String::RemoveWhitespaces()
    {
        char* result = new char[GetLength() + 1];
        result[GetLength()] = 0;
        bool isString = false;
        int idx = 0;

        for (int i = 0; i < GetLength(); i++)
        {
            if (Buffer[i] == '"')
            {
                isString = !isString;
                continue;
            }

            if (Buffer[i] == ' ' && !isString)
            {
                continue;
            }

            result[idx++] = Buffer[i];
        }

        result[idx++] = 0;

        auto str = String(result);
        delete result;
        return str;
    }

	void String::UpdateLength()
	{
		StringLength = Math::Min((u32)std::strlen(Buffer), Capacity);
	}

    bool String::TryParse(const String& string, u8& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<u8>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, s8& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<s8>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, u32& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<u32>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, s32& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<s32>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, u64& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<u64>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, s64& outValue)
    {
        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = StaticCast<s64>(value);
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, f32& outValue)
    {
        f32 value = 0;
        if (TryParseFloat(string, value))
        {
            outValue = value;
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, f64& outValue)
    {
        f64 value = 0;
        if (TryParseFloat(string, value))
        {
            outValue = value;
            return true;
        }
        return false;
    }

    bool String::TryParse(const String& string, b8& outValue)
    {
        if (string == "true" || string == "True")
        {
            outValue = true;
            return true;
        }
        else if (string == "false" || string == "False")
        {
            outValue = false;
            return true;
        }

        s64 value = 0;
        if (TryParseInteger(string, value))
        {
            outValue = value > 0;
            return true;
        }

        return false;
    }

    void String::Internal_ThrowParseException(const char* message)
    {
        throw ParseFailedException(message);
    }

    bool String::TryParseInteger(String value, s64& outValue)
    {
        if (value.IsEmpty())
            return false;
        
        value = value.RemoveWhitespaces();
        
        if (value.Contains('.') || value.Contains('f'))
            return false;

        s64 result = 0;

        int length = value.GetLength();
        bool isNegative = false;

        for (int i = length - 1; i >= 0; i--)
        {
            if (i > 0)
            {
                if (!IsNumeric(value[i]))
                    return false;
            }
            else
            {
                if (!IsNumeric(value[i]) && value[i] != '+' && value[i] != '-')
                    return false;
                if (value[i] == '+' || value[i] == '-')
                {
                    isNegative = value[i] == '-';
                    continue;
                }
            }
            
            result += Math::Pow((s64)10, length - 1 - i) * CharToNumber(value[i]);
        }

        if (isNegative)
            result *= -1;

        outValue = result;
        
        return true;
    }

    bool String::TryParseFloat(String value, f32& outValue)
    {
        if (value.IsEmpty())
            return false;
        
        value = value.RemoveWhitespaces();

        try
        {
            outValue = std::stof(value.ToStdString());
        }
        catch (...)
        {
            return false;
        }
        
        return true;
    }

    bool String::TryParseFloat(String value, f64& outValue)
    {
        if (value.IsEmpty())
            return false;
        
        value = value.RemoveWhitespaces();
        
        try
        {
            outValue = std::stod(value.ToStdString());
        }
        catch (...)
        {
            return false;
        }
        
        return true;
    }
}


