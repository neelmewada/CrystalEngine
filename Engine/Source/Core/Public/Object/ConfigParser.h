#pragma once

#include "IO/Stream/GenericStream.h"
#include "IO/Stream/MemoryStream.h"

namespace CE
{
    class TypeInfo;
    class ClassType;
    class StructType;
    class Object;

    class CORE_API ConfigParser
    {
    public:
        ConfigParser(StructType* structType);

        void Parse(void* structInstance, CE::IO::Path filePath);

    private:
        StructType* structType = nullptr;
    };
    
} // namespace CE
