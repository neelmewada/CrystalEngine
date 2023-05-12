
#include "CoreMinimal.h"

namespace CE
{

    StructuredStream::StructuredStream(StructuredStreamFormatter& formatter) : formatter(formatter)
    {
        
    }

    bool StructuredStream::CanRead()
    {
        return IsOpen() && formatter.CanRead();
    }

    bool StructuredStream::CanWrite()
    {
        return IsOpen() && formatter.CanWrite();
    }

    bool StructuredStream::IsOpen()
    {
        return formatter.IsValid() && GetUnderlyingStream().IsOpen();
    }

    Stream& StructuredStream::GetUnderlyingStream()
    {
        return formatter.GetUnderlyingStream();
    }

    StructuredStream& StructuredStream::operator<<(StructuredStreamInstruction writeInstruction)
    {
        if (!CanWrite())
            return *this;

        switch (writeInstruction)
        {
        case BeginMap: formatter.EnterMap(); break;
        case EndMap: formatter.ExitMap(); break;
        case BeginArray: break;
        case EndArray: break;
        }

        return *this;
    }

    StructuredStream& StructuredStream::operator<<(const String& stringValue)
    {
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(f64 numberValue)
    {
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(s32 numberValue)
    {
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(s64 numberValue)
    {
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(bool boolValue)
    {
        return *this;
    }
} // namespace CE

