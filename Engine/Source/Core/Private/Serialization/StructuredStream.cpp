
#include "CoreMinimal.h"

namespace CE
{

    StructuredStream::StructuredStream(StructuredStreamFormatter& formatter) : formatter(formatter)
    {
        
    }

    bool StructuredStream::CanRead() const
    {
        return formatter.CanRead();
    }

    bool StructuredStream::CanWrite() const
    {
        return formatter.CanWrite();
    }
    
} // namespace CE

