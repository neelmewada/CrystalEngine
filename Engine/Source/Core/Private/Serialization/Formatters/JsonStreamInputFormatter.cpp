
#include "CoreMinimal.h"

namespace CE
{
    static StructuredStreamEntry invalidRoot{};
    
    static void FromJson(JsonValue& jsonValue, StructuredStreamEntry& outEntry, StructuredStreamPosition& position)
    {
        if (jsonValue.IsArrayValue())
        {
            outEntry = StructuredStreamArray();
            outEntry.positionStack = position;
        }
        else if (jsonValue.IsObjectValue())
        {
            outEntry = StructuredStreamMap();
            outEntry.positionStack = position;
        }
        else if (jsonValue.IsStringValue())
        {
            outEntry = StructuredStreamField(jsonValue.GetStringValue());
            outEntry.positionStack = position;
        }
        else if (jsonValue.IsNumberValue())
        {
            outEntry = StructuredStreamField(jsonValue.GetNumberValue());
            outEntry.positionStack = position;
        }
        else if (jsonValue.IsBoolValue())
        {
            outEntry = StructuredStreamField(jsonValue.GetBoolValue());
            outEntry.positionStack = position;
        }
        else if (jsonValue.IsNullValue())
        {
            outEntry = StructuredStreamField();
            outEntry.positionStack = position;
        }
    }

    static void PopulateStructures(JsonValue& rootJson, StructuredStreamEntry& rootEntry)
    {
        if (rootJson.IsArrayValue() && rootEntry.IsArray())
        {
            auto& arrayStructure = static_cast<StructuredStreamArray&>(rootEntry);
            for (int i = 0; i < rootJson.GetArrayValue().GetSize(); i++)
            {
                auto& item = *rootJson.GetArrayValue()[i];
                if (item.IsStringValue())
                {
                    arrayStructure.array.Add(new StructuredStreamField(item.GetStringValue()));
                }
                else if (item.IsNumberValue())
                {
                    arrayStructure.array.Add(new StructuredStreamField(item.GetNumberValue()));
                }
                else if (item.IsBoolValue())
                {
                    arrayStructure.array.Add(new StructuredStreamField(item.GetBoolValue()));
                }
                else if (item.IsNullValue())
                {
                    arrayStructure.array.Add(new StructuredStreamField());
                }
                else if (item.IsArrayValue())
                {
                    
                }
            }
        }
        else if (rootJson.IsObjectValue())
        {
            
        }
    }
    
    JsonStreamInputFormatter::JsonStreamInputFormatter(Stream& stream)
        : stream(stream), rootEntry(invalidRoot), currentEntry(invalidRoot)
    {
        if (!stream.CanRead())
            throw StructuredStreamFormatterException("JsonStreamInputFormatter passed with a stream that cannot be written to!");

        // Load in Json values
        rootJson = JsonSerializer::Deserialize(&stream);

        if (rootJson->IsArrayValue())
        {
            rootEntry = StructuredStreamArray();
            rootEntry.formatter = this;
        }
        else if (rootJson->IsObjectValue())
        {
            rootEntry = StructuredStreamMap();
            rootEntry.formatter = this;
        }
        
        //PopulateStructures(*rootValue, rootEntry);
        currentEntry = rootEntry;
    }

    JsonStreamInputFormatter::~JsonStreamInputFormatter()
    {
        delete rootJson;
    }

    Stream& JsonStreamInputFormatter::GetUnderlyingStream()
    {
        return stream;
    }

    bool JsonStreamInputFormatter::CanRead()
    {
        return true;
    }

    bool JsonStreamInputFormatter::CanWrite()
    {
        return false;
    }

    bool JsonStreamInputFormatter::IsValid()
    {
        return stream.CanRead();
    }

    bool JsonStreamInputFormatter::IsRoot()
    {
        return valueStack.GetSize() == 0;
    }

    bool JsonStreamInputFormatter::TryEnterMap()
    {
        if (GetNextEntryType() != StructuredStreamEntry::Type::Map)
            return false;
        EnterMap();
        return true;
    }

    bool JsonStreamInputFormatter::TryEnterArray()
    {
        if (GetNextEntryType() != StructuredStreamEntry::Type::Array)
            return false;
        EnterArray();
        return true;
    }

    bool JsonStreamInputFormatter::TryEnterField(const String& identifier)
    {
        if (GetNextEntryType() != StructuredStreamEntry::Type::Field)
            return false;
        EnterField(identifier);
        return true;
    }

    void JsonStreamInputFormatter::EnterMap()
    {
        if (valueStack.GetSize() == 0)
        {
            valueStack.Push(rootJson);
            return;
        }
        
        if (valueStack.Top()->IsObjectValue() && !currentIdentifier.IsEmpty())
        {
            valueStack.Push(valueStack.Top()->GetObjectValue()[currentIdentifier]);
        }
        else if (valueStack.Top()->IsArrayValue())
        {
            valueStack.Push(valueStack.Top()->GetArrayValue()[currentArrayIndex]);
        }
    }
    
    void JsonStreamInputFormatter::ExitMap()
    {
        valueStack.Pop();
        positionStack.Pop();
    }
    
    void JsonStreamInputFormatter::EnterArray()
    {
        if (valueStack.GetSize() == 0)
        {
            valueStack.Push(rootJson);
            return;
        }
        
        if (valueStack.Top()->IsObjectValue() && !currentIdentifier.IsEmpty())
        {
            valueStack.Push(valueStack.Top()->GetObjectValue()[currentIdentifier]);
        }
        else if (valueStack.Top()->IsArrayValue())
        {
            valueStack.Push(valueStack.Top()->GetArrayValue()[currentArrayIndex]);
        }
    }

    void JsonStreamInputFormatter::ExitArray()
    {
        valueStack.Pop();
    }

    void JsonStreamInputFormatter::EnterField(const String& identifier)
    {
        if (valueStack.GetSize() == 0)
            return;

        if (valueStack.Top()->IsObjectValue())
        {
            valueStack.Push(valueStack.Top()->GetObjectValue()[identifier]);
        }
        else if (valueStack.Top()->IsArrayValue())
        {
            valueStack.Push(valueStack.Top()->GetArrayValue()[currentArrayIndex]);
        }
    }

    void JsonStreamInputFormatter::ExitField()
    {
        
    }

    void JsonStreamInputFormatter::EnterStringValue(const String& value)
    {
        
    }

    void JsonStreamInputFormatter::EnterNumberValue(f64 value)
    {
        
    }

    void JsonStreamInputFormatter::EnterBoolValue(bool value)
    {
        
    }

    void JsonStreamInputFormatter::EnterNullValue()
    {
        
    }

    StructuredStreamEntry& JsonStreamInputFormatter::GetRootEntry()
    {
        return rootEntry;
    }

    StructuredStreamEntry& JsonStreamInputFormatter::GetEntryAt(StructuredStreamPosition position)
    {
        static StructuredStreamEntry invalid{};
        
        auto& jsonEntry = *rootJson;
        auto& structureEntry = rootEntry;
        Array<StructuredStreamPrivate::EntryKey> curPosStack{};
        
        for (int i = 0; i < position.GetSize(); i++)
        {
            auto curPos = position[i];
            curPosStack.Add(curPos);
            if (curPos.IsIndex() && jsonEntry.IsArrayValue() && curPos.index < jsonEntry.GetArrayValue().GetSize())
            {
                jsonEntry = jsonEntry[curPos.index];
                FromJson(jsonEntry, structureEntry, curPosStack);
            }
            else if (curPos.IsKey() && jsonEntry.IsObjectValue() && jsonEntry.GetObjectValue().KeyExists(curPos.key))
            {
                jsonEntry = jsonEntry[curPos.key];
                FromJson(jsonEntry, structureEntry, curPosStack);
            }
            else
            {
                return invalid;
            }
        }

        return structureEntry;
    }

    StructuredStreamEntry::Type JsonStreamInputFormatter::GetNextEntryType()
    {
        if (!currentEntry.IsValid())
            return StructuredStreamEntry::Type::None;

        if (currentEntry.IsArray())
            return StructuredStreamEntry::Type::Array;
        if (currentEntry.IsMap())
            return StructuredStreamEntry::Type::Map;
        if (currentEntry.IsField())
            return StructuredStreamEntry::Type::Field;
        
        return StructuredStreamEntry::Type::None;
    }
}
