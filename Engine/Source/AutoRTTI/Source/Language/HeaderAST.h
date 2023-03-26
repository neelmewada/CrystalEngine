#pragma once

#include "HeaderTokenizer.h"

namespace CE
{

    class HeaderAST
    {
    private:
        HeaderAST();
    public:
        virtual ~HeaderAST();

        HeaderTokenizer* tokens = nullptr;

        friend class ModuleAST;
    };
    
} // namespace CE
