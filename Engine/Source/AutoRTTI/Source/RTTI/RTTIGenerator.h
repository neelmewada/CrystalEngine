#pragma once

#include "CoreMinimal.h"

#include "Language/HeaderTokenizer.h"
#include "Language/HeaderAST.h"
#include "Language/ModuleAST.h"

namespace CE
{

    class RTTIGenerator
    {
    private:
        RTTIGenerator() = delete;
    public:

        static void GenerateRTTI(String moduleName, IO::Path moduleHeaderRootPath, IO::Path outputPath);

        static Array<IO::Path> includeSearchPaths;
    };
    
} // namespace CE
