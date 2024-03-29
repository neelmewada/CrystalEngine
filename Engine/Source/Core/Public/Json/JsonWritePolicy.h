#pragma once

namespace CE
{

    class JsonWritePolicy
    {
    public:

        FORCE_INLINE static void WriteChar(Stream* stream, char ch)
        {
            stream->Write(&ch, sizeof(ch));
        }

        FORCE_INLINE static void WriteString(Stream* stream, const String& string)
        {
            stream->Write(string);
        }

        FORCE_INLINE static void WriteFloat(Stream* stream, f32 value)
        {
            WriteString(stream, String::Format("{}", value));
        }

        FORCE_INLINE static void WriteBool(Stream* stream, bool value)
        {
            if (value)
                WriteString(stream, "true");
            else
                WriteString(stream, "false");
        }
    };

    class JsonPrettyPrintPolicy : public JsonWritePolicy
    {
    public:

        FORCE_INLINE static void WriteLineEnding(Stream* stream)
        {
            WriteChar(stream, '\n');
        }

        FORCE_INLINE static void WriteLineTabs(Stream* stream, int count)
        {
            for (int i = 0; i < count; i++)
            {
                WriteChar(stream, '\t');
            }
        }

        FORCE_INLINE static void WriteWhiteSpace(Stream* stream, int count = 1)
        {
            for (int i = 0; i < count; i++)
            {
                WriteChar(stream, ' ');
            }
        }
    };

    class JsonCondensedPrintPolicy : public JsonWritePolicy
    {
    public:

        FORCE_INLINE static void WriteLineEnding(Stream* stream)
        {
            
        }

        FORCE_INLINE static void WriteLineTabs(Stream* stream, int count)
        {
            
        }

        FORCE_INLINE static void WriteWhiteSpace(Stream* stream, int count = 0)
        {
            
        }
    };
    
}
