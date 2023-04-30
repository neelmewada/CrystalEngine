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
    
} // namespace CE
