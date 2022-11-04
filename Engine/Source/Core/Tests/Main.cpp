
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

#define EXPORT_API


namespace Test::Child
{
    
    class BaseClass
    {
        CE_RTTI(BaseClass);
    public:
        int a = 1;
    };

    class BaseClass2
    {
        CE_RTTI(BaseClass2);
    public:
        int b = 2;
    };

    class Derived : public BaseClass, public BaseClass2
    {
        CE_RTTI(Derived, BaseClass);
    public:
        int c = 3;
    };
}


CE_RTTI_DECLARE_CLASS(Test::Child::BaseClass);
CE_RTTI_IMPLEMENT_CLASS(Test::Child::BaseClass);

CE_RTTI_DECLARE_CLASS(Test::Child::BaseClass2);
CE_RTTI_IMPLEMENT_CLASS(Test::Child::BaseClass2);

CE_RTTI_DECLARE_CLASS(Test::Child::Derived, Test::Child::BaseClass, Test::Child::BaseClass2);
CE_RTTI_IMPLEMENT_CLASS(Test::Child::Derived);



int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    using namespace Test::Child;

    CE_LOG(Info, All, "Id: {}", GetTypeId<s32>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::String>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Object>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Object*>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<const CE::Object*>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Object const*>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<CE::Object* const>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<int>());
    CE_LOG(Info, All, "Id: {}", GetTypeId<Derived>());
    
    auto type = Derived::Type();

    CE_LOG(Info, All, "Name: {} | Id: {}", type->GetName(), type->GetTypeId());

    /*CE_LOG(Info, All, "Prop: {}", type->GetPropertyAt(0)->GetName());

    auto prop1 = type->GetPropertyAt(0);
    auto prop2 = type->GetPropertyAt(1);

    TestClass* testClass = new TestClass();
    testClass->Prop1 = "Old value";
    testClass->Prop2 = 412.215f;

    CE_LOG(Info, All, "Value: {} {}", prop1->GetValue<String>(testClass), prop2->GetValue<f32>(testClass));

    prop1->SetValue<String>(testClass, "New string");
    prop2->SetValue<f32>(testClass, 824.2124f);

    CE_LOG(Info, All, "Value: {} {}", testClass->Prop1, testClass->Prop2);*/

    //auto func1 = type->GetFunctionAt(0);

    //CE_LOG(Info, All, "Func name: {}", func1->GetName());

    //std::any result = func1->Invoke(testClass, { (s32)45, String("hello") });
    //CE::String resultString = std::any_cast<CE::String>(result);

    //CE_LOG(Info, All, "Result: {}", resultString);
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

