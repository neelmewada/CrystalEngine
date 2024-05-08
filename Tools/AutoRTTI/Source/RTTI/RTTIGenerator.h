#pragma once

#include "Core.h"

#include "Language/HeaderTokenizer.h"
#include "Language/HeaderAST.h"
#include "Language/ModuleAST.h"

namespace CE
{

    // Globals

    extern bool gShouldEmitApiMacro;

    struct HeaderCRC
    {
        CE_STRUCT(HeaderCRC)
    public:
        HeaderCRC() = default;
        HeaderCRC(IO::Path headerPath, u32 crc)
            : headerPath(headerPath), crc(crc)
        {}


        IO::Path headerPath{};
        u32 crc = 0;
    };

    struct ModuleStamp
    {
        CE_STRUCT(ModuleStamp)
    public:
        CE::Array<HeaderCRC> headers{};
    };
    

    class RTTIGenerator
    {
    private:
        RTTIGenerator() = delete;
    public:

        static void GenerateRTTI(String moduleName, IO::Path moduleHeaderRootPath, IO::Path outputPath, ModuleStamp& moduleStamp);

        static Array<IO::Path> includeSearchPaths;
    };

    
} // namespace CE

CE_RTTI_STRUCT(, CE, HeaderCRC,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(headerPath)
        CE_FIELD(crc)
    ),
    CE_FUNCTION_LIST()
)


CE_RTTI_STRUCT(, CE, ModuleStamp,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(headers)
    ),
    CE_FUNCTION_LIST()
)
