#pragma once

#include "StructuredStreamFormatter.h"
#include "StructuredStreamEntries.h"

namespace CE
{

    class CORE_API StructuredStream
    {
    public:
        using Entry = StructuredStreamEntry;
        
        StructuredStream(StructuredStreamFormatter& formatter);

    private:
        StructuredStreamFormatter& formatter;
    };
    
} // namespace CE
