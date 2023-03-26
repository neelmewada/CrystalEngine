#pragma once

#include "HeaderTokenizer.h"

namespace CE
{
    struct FieldInfo
    {
        Name name{};
        Array<String> attribs{};
    };

    struct StructInfo
    {
        Name nameSpace{};
        Name name{};
        Array<Name> superClasses;
        Array<String> attribs{};
        Array<FieldInfo> fields{};
    };

    struct ClassInfo : public StructInfo
    {
        
    };

    class HeaderAST
    {
    private:
        HeaderAST();
    public:
        virtual ~HeaderAST();
        
        static HeaderAST* ProcessHeader(HeaderTokenizer* tokens);
        
        Array<ClassInfo> classes{};

        friend class ModuleAST;
    };
    
} // namespace CE
