#pragma once

#include "StructuredStreamFormatter.h"
#include "StructuredStreamEntries.h"

namespace CE
{

    class CORE_API StructuredStream
    {
    public:
        using Entry = StructuredStreamEntry;
        using Array = StructuredStreamArray;
        
        StructuredStream(StructuredStreamFormatter& formatter);

        bool CanRead() const;
        bool CanWrite() const;

    private:
        StructuredStreamFormatter& formatter;
    };
    
} // namespace CE
