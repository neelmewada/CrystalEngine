#pragma once

#include "CoreMinimal.h"

#include "HeaderAST.h"

namespace CE
{

    class ModuleAST
    {
    public:
        ModuleAST(String moduleName);
        ~ModuleAST();

        void ProcessHeader(IO::Path headerPath, const Array<IO::Path>& includeSearchPaths);

    private:
        Array<HeaderAST*> processedHeaders{};

        String moduleName{};
    };
    
} // namespace CE
