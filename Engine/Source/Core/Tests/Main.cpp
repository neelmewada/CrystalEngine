
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
        CE_RTTI_CLASS(Some0);
    public:
        int s0 = 0;
        CE::String baseString;
    };

    class BaseClass : public Some0
    {
        CE_RTTI_CLASS(BaseClass);
    public:
        CE::String childString;
    };

}

CE_RTTI_DECLARE_CLASS(Test::Child::Some0, 
    CE_SUPER(),
    CE_FIELD_LIST(
        CE_FIELD(s0)
        CE_FIELD(baseString, CustomAttribute, MaxLength = 32)
    )
);

CE_RTTI_IMPLEMENT_CLASS(Test::Child::Some0);


CE_RTTI_DECLARE_CLASS(Test::Child::BaseClass,
    CE_SUPER(Test::Child::Some0),
    CE_FIELD_LIST(
        CE_FIELD(childString)
    )
);

CE_RTTI_IMPLEMENT_CLASS(Test::Child::BaseClass);



int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    using namespace Test::Child;

    CE_LOG(Info, All, "Id: {}", GetTypeId<s32>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::String>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Object*>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Array<void>>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Array<CE::String>>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Array<s32>>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<int>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<BaseClass>());
    
    auto type = BaseClass::Type();

    CE_LOG(Info, All, "Name: {} | Id: {} | {}", type->GetName(), type->GetTypeId(), type->IsClass());
    
    type = Some0::Type();

    CE_LOG(Info, All, "Name: {} | Id: {} | {}", type->GetName(), type->GetTypeId(), type->IsClass());

    CE_LOG(Info, All, "Field Count: {}", type->GetFieldCount());

    for (int i = 0; i < type->GetFieldCount(); i++)
    {
        CE_LOG(Info, All, "Field {}: {} | typeId: {} | ({}, {}) | Attrs: {}", i, type->GetFieldAt(i)->GetName(), type->GetFieldAt(i)->GetTypeId(), 
            type->GetFieldAt(i)->GetSize(), type->GetFieldAt(i)->GetOffset(),
            type->GetFieldAt(i)->GetRawAttributes());
    }
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

