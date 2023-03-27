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

        void ProcessHeader(IO::Path headerPath, const Array<IO::Path>& includeSearchPaths, 
            std::stringstream& outStream, std::stringstream& implStream, CE::Array<String>& registrantList);

    private:
        Array<HeaderAST*> processedHeaders{};

        String moduleName{};
    };
    
} // namespace CE
