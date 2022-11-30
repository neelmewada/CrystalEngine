
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

#define EXPORT_API


namespace Test::Child
{
    class Some0
    {
        CE_RTTI(Some0);
    private:
        int s0 = 0;
        CE::String baseString;

        void PrintHello()
        {
            CE_LOG(Info, All, "Hello world from Some0");
        }
    };

    class BaseClass : public Some0
    {
        CE_RTTI(BaseClass);
    public:
        CE::String childString;
    };

    enum class MyEnum
    {
        None,
        Opt1,
        Opt2 = 1 << 3,
        Opt3,
        Opt4,
        Opt5 = Opt1 | Opt4,
    };
}

CE_RTTI_ENUM(, Test::Child, MyEnum,
    CE_ATTRIBS(Flags, DontSerialize),
    CE_CONST(None),
    CE_CONST(Opt1),
    CE_CONST(Opt2),
    CE_CONST(Opt3),
    CE_CONST(Opt4),
    CE_CONST(Opt5),
);

CE_RTTI_ENUM_IMPL(, Test::Child, MyEnum);

CE_RTTI_CLASS(,Test::Child, Some0,
    CE_SUPER(),
    CE_ATTRIBS(Abstract),
    CE_FIELD_LIST(
        CE_FIELD(s0)
        CE_FIELD(baseString, CustomAttribute, MaxLength = 32)
    )
);

CE_RTTI_CLASS_IMPL(,Test::Child, Some0);


CE_RTTI_CLASS(,Test::Child, BaseClass,
    CE_SUPER(Test::Child::Some0),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(childString)
    )
);

CE_RTTI_CLASS_IMPL(,Test::Child, BaseClass);



int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    using namespace Test::Child;
    
    //auto type = BaseClass::Type();

    CE_LOG(Info, All, "s8: {:X}", TYPEID(CE::s8));
    CE_LOG(Info, All, "s16: {:X}", TYPEID(CE::s16));
    CE_LOG(Info, All, "s32: {:X}", TYPEID(CE::s32));
    CE_LOG(Info, All, "s64: {:X}", TYPEID(CE::s64));
    CE_LOG(Info, All, "Typeid: {:X}", TYPEID(CE::Array<s32>));
    CE_LOG(Info, All, "Typeid: {:X}", TYPEID(CE::Array<CE::String>));
    CE_LOG(Info, All, "Typeid: {:X}", TYPEID(const s32*));
    CE_LOG(Info, All, "Typeid: {:X}", TYPEID(s32*));
    CE_LOG(Info, All, "Typeid: {:X}", TYPEID(CE::String));
    
    CE_LOG(Info, All, "MyEnum: {:X}", TYPEID(MyEnum));
    CE_LOG(Info, All, "MyEnum: {:X}", TYPEID(__underlying_type(MyEnum))); // int = s32
    CE_LOG(Info, All, "MyEnum: {:X}", TYPEID(s32));
    CE_LOG(Info, All, "MyEnum: {:X}", TYPEID(u32));
    
    CE_LOG(Info, All, "Some0: {}", TYPEID(Test::Child::Some0));
    CE_LOG(Info, All, "Base Class: {}", TYPEID(Test::Child::BaseClass));
    
    CE_LOG(Info, All, "Registered: {}", TypeInfo::GetRegisteredCount());
    
    auto type = GetTypeInfo(TYPEID(Test::Child::BaseClass));
    
    if (type != nullptr)
    {
        auto name = type->GetName();

        CE_LOG(Info, All, "Type found: {} | {}", type->IsClass(), name);

        CE_LOG(Info, All, "Names {}: {} ; {} ; {}", name.GetComponentCount(), name.GetComponentAt(0), name.GetComponentAt(1), name.GetComponentAt(2));

        auto classType = (ClassType*)type;
        auto field = classType->GetFirstField();

        while (field != nullptr)
        {
            CE_LOG(Info, All, "Field: {} ; {}", field->GetName(), field->GetOffset());

            field = field->GetNext();
        }
    }
    else
    {
        CE_LOG(Error, All, "Type not found!");
    }

    type = GetTypeInfo(TYPEID(Test::Child::MyEnum));

    if (type != nullptr)
    {
        auto enumType = (EnumType*)type;

        CE_LOG(Info, All, "Enum: {}", enumType->GetName(), enumType->GetConstantsCount());

        for (int i = 0; i < enumType->GetConstantsCount(); i++)
        {
            CE_LOG(Info, All, "Constant {}: {} = {:b}", i, enumType->GetConstant(i)->GetName(), enumType->GetConstant(i)->GetValue());
        }
    }
    else
    {
        CE_LOG(Error, All, "Enum Type not found!");
    }
    
    //CE_LOG(Info, All, "Name: {} | Id: {} | {} | Super Count: {} | {}", type->GetName(), type->GetTypeId(), type->IsClass(), type->GetSuperTypesCount(), type->GetLocalFunctionCount());
    
    //auto type = Some0::Type();

    //CE_LOG(Info, All, "Name: {} | Id: {} | {} | Super Count: {} | {}", type->GetName(), type->GetTypeId(), type->IsClass(), type->GetSuperTypesCount(), type->GetLocalFunctionCount());

    //CE_LOG(Info, All, "Field Count: {}", type->GetLocalFieldCount());
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

