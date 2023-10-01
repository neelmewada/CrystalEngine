#pragma once

#include "HeaderTokenizer.h"

namespace CE
{
    struct FieldInfo
    {
        Name name{};
        Array<String> attribs{};
    };

    struct FunctionInfo
    {
        Name name{};
        String signature{};
        String returnType = "auto";
        bool isSignal = false;
        bool isEvent = false;
        Array<String> attribs{};
    };

    struct StructInfo
    {
        Name nameSpace{};
        Name name{};
        Array<Name> superClasses;
        Array<String> attribs{};
        Array<FieldInfo> fields{};
		Array<FunctionInfo> functions{};
    };

    struct ClassInfo : public StructInfo
    {
        
    };

    struct EnumConstInfo
    {
        Name name{};
        Array<String> attribs{};
    };

    struct EnumInfo
    {
        Name nameSpace{};
        Name name{};
        Array<EnumConstInfo> constants{};
        Array<String> attribs{};
    };

    class HeaderAST
    {
    private:
        HeaderAST();
    public:
        virtual ~HeaderAST();
        
        static HeaderAST* ProcessHeader(HeaderTokenizer* tokens);
        
        Array<ClassInfo> classes{};
        Array<StructInfo> structs{};
        Array<EnumInfo> enums{};

        friend class ModuleAST;
    };
    
} // namespace CE
