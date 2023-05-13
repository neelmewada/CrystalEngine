
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
        case Null: formatter.EnterNullValue(); break;
        case Key: pendingInstruction = Key; break;
        case Value: pendingInstruction = Value; break;
        case BeginMap: formatter.EnterMap(); break;
        case EndMap: formatter.ExitMap(); break;
        case BeginArray: formatter.EnterArray(); break;
        case EndArray: formatter.ExitArray(); break;
        }

        return *this;
    }

    StructuredStream& StructuredStream::operator<<(const String& stringValue)
    {
        if (pendingInstruction == Key)
        {
            pendingInstruction = Null;
            formatter.EnterField(stringValue);
        }
        else if (pendingInstruction == Value)
        {
            pendingInstruction = Null;
            formatter.EnterStringValue(stringValue);
            formatter.ExitField();
        }
        else
        {
            formatter.EnterStringValue(stringValue);
        }
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(f64 numberValue)
    {
        if (pendingInstruction == Value)
        {
            pendingInstruction = Null;
            formatter.EnterNumberValue(numberValue);
            formatter.ExitField();
        }
        else
        {
            formatter.EnterNumberValue(numberValue);
        }
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(s32 numberValue)
    {
        if (pendingInstruction == Value)
        {
            pendingInstruction = Null;
            formatter.EnterNumberValue(numberValue);
            formatter.ExitField();
        }
        else
        {
            formatter.EnterNumberValue(numberValue);
        }
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(s64 numberValue)
    {
        if (pendingInstruction == Value)
        {
            pendingInstruction = Null;
            formatter.EnterNumberValue(numberValue);
            formatter.ExitField();
        }
        else
        {
            formatter.EnterNumberValue(numberValue);
        }
        return *this;
    }

    StructuredStream& StructuredStream::operator<<(bool boolValue)
    {
        if (pendingInstruction == Value)
        {
            pendingInstruction = Null;
            formatter.EnterBoolValue(boolValue);
            formatter.ExitField();
        }
        else
        {
            formatter.EnterBoolValue(boolValue);
        }
        return *this;
    }

    static StructuredStreamEntry invalidEntry{};
    
    StructuredStreamEntry& StructuredStreamEntry::operator[](u32 index)
    {
        return invalidEntry;
    }

    StructuredStreamEntry& StructuredStreamEntry::operator[](const String& key)
    {
        return invalidEntry;
    }

    StructuredStreamArray::~StructuredStreamArray()
    {
        for (auto entry : array)
        {
            delete entry;
        }
        array.Clear();
    }

    StructuredStreamEntry& StructuredStreamArray::operator[](u32 index)
    {
        return *array[index];
    }

    StructuredStreamMap::~StructuredStreamMap()
    {
        for (auto [_, entry] : map)
        {
            delete entry;
        }
        map.Clear();
    }

    StructuredStreamEntry& StructuredStreamMap::operator[](const String& key)
    {
        return *map.Get(key);
    }
    
} // namespace CE

