#pragma once

#include "CoreTypes.h"

#include <exception>

namespace CE
{

    class CORE_API Exception : public std::exception
    {
    public:
        Exception() : message("Unknown error")
        {}

        Exception(const String& message) : message(message)
        {}

        const char* what() const throw () {
            return message.GetCString();
        }

    private:
        String message{};
    };

    class CORE_API ParseFailedException : public Exception
    {
    public:
        ParseFailedException() : Exception("Failed to parse given input")
        {}

        ParseFailedException(const String& message) : Exception(message)
        {}
    };

    class CORE_API NullPointerException : public Exception
    {
    public:
        NullPointerException() : Exception("NullPointerException")
        {}

        NullPointerException(const String& message) : Exception("NullPointerException: " + message)
        {}

    };

    class CORE_API IndexOutOfRangeException : public Exception
    {
    public:

        IndexOutOfRangeException() : Exception("IndexOutOfRangeException")
        {}

        IndexOutOfRangeException(const String& message) : Exception("IndexOutOfRangeException: " + message)
        {}
    };
    
} // namespace CE
