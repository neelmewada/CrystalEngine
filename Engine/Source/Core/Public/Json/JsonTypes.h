#pragma once

namespace CE
{

    enum class JsonValueType
    {
        None,
        Null,
        Object,
        Array,
        Boolean,
        String,
        Number
    };

    enum class JsonToken
    {
        None,
        Identifier,
        CurlyOpen,
        CurlyClose,
        SquareOpen,
        SquareClose,
        Comma,
        Colon,

        // Value tokens
        Number,
        String,
        True,
        False,
        Null
    };

    enum class JsonNotation
    {
        None,
        ObjectStart,
        ObjectEnd,
        ArrayStart,
        ArrayEnd
    };
    
} // namespace CE
