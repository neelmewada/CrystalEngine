#pragma once

#include "CoreTypes.h"

namespace CE
{

    /*
     *  Base class for all types of streams. Ex: String stream, memory stream, file stream, structured stream, etc
     */
    class CORE_API Stream
    {
    public:
        Stream() = default;
        Stream(const Stream&) = default;
        Stream& operator=(const Stream&) = default;
        ~Stream() = default;

        virtual Stream& operator<<(String& name);
    };
    
} // namespace CE
