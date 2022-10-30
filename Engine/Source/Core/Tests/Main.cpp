
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

#define EXPORT_API

#define MACRO_TEST(x, y, z) x, z, y
#define STRINGIFY(x) #x
#define MACRO2(x) STRINGIFY(x)

namespace Test::Child
{

    class TestClass : public CE::Object
    {
    public:
        CE_META_CLASS(Test::Child, TestClass, CE::Object);

        TestClass() : CE::Object()
        {
            MACRO2(MACRO_TEST(1, 2, 3));
        }

    //private:

        CE::String Prop1;

        float Prop2;

        CE::Object* ObjRef = nullptr;

        String ExposedFunction(s32 integer, String extra)
        {
            return String::Format("Formated string: {} {}", integer, extra);
        }

        void ExposedFunc2(String value)
        {
            CE_LOG(Info, All, "Test Function: {}", value);
        }
    };
    
    // Header file
    CE_META_DECLARE_CLASS(Test::Child, TestClass, EXPORT_API, CE::Object,
        ,//CE_EXTRA_SUPER_CLASS(SomeSuperclass, ISomeInterface),
        CE_ATTRIBUTES(Hidden, Max = 412, DontSave),

        CE_PROPERTY_LIST(
            CE_PROPERTY(Prop1, Plain, String, CE::String)
            CE_PROPERTY(Prop2, Plain, f32, CE::f32)
        ),

        CE_FUNCTION_LIST(
            CE_FUNCTION(ExposedFunction,
                CE_PARAM_LIST(
                    CE_PARAM(0, integer, CE::s32, Plain, s32, "", ""),
                    CE_PARAM(1, extra, CE::String, Plain, String, "", "")
                )
            )
        )
    )

    // Cpp file
    CE_META_IMPLEMENT_CLASS(TestClass, EXPORT_API);

    void Example()
    {
        CE_FUNCTION(ExposedFunction,
            CE_PARAM_LIST(
                CE_PARAM(0, integer, CE::s32, Plain, s32, "", ""),
                CE_PARAM(1, extra, CE::String, Plain, String, "", "")
            )
        )
    }
    
    
    // Header file
    /*class EXPORT_API CE_Generated_MetaClass_TestClass_Singleton : public CE::ClassType // Begin meta class
    {
    private:
        CE_Generated_MetaClass_TestClass_Singleton();
        ~CE_Generated_MetaClass_TestClass_Singleton();

    public:
        typedef TestClass Type; // typedef ClassType Type;

        static CE_Generated_MetaClass_TestClass_Singleton& Get()
        {
            static CE_Generated_MetaClass_TestClass_Singleton instance;
            return instance;
        }

    private:
        static constexpr const char* _Name = "TestClass";
        static constexpr CE::FieldType _Type = CE::FieldType::Plain;
        static constexpr CE::FieldBaseType _UnderlyingType = CE::FieldBaseType::Class;
        static constexpr const char* _ClassPath = "Test::Child" "::" "TestClass";
        static constexpr const char* _Attributes = "SomeAttribute,DontSave,EditAnywhere";

        void SetupProperties() // Begin props
        {
            Properties.Add(new CE::PropertyType(CE::PropertyInitializer{
                "Prop1", _ClassPath, offsetof(Type, Prop1), sizeof(Type::Prop1), CE::FieldType::Plain, CE::FieldBaseType::String, "CE::String", "Hidden,Default=Type here...,"
                }));

        } // End props

        void SetupFunctions()
        {
            using ExposedFunction0 = s32;
            using ExposedFunction1 = CE::String;
            
            Functions.Add(new CE::FunctionType(CE::FunctionInitializer
                {
                    "ExposedFunction", TEXT(Test::Child::TestClass::ExposedFunction), false, 
                    [&](void* instance, std::initializer_list<std::any> params, std::any& result) -> void {
                        Type* ptr = (Type*)instance;
                        result = ptr->ExposedFunction(
                            std::any_cast<CE::s32>(*(params.begin() + 0)),
                            std::any_cast<CE::String>(*(params.begin() + 1))
                        );
                    },
                    // Parameters
                    {
                        new CE::ParameterType({ "integer", false, sizeof(s32), CE::FieldType::Plain, CE::FieldBaseType::s32, "CE::s32", "SomeAttribute,Hidden,Max=12" }),
                        new CE::ParameterType({ "extra", false, sizeof(CE::String), CE::FieldType::Plain, CE::FieldBaseType::String, "CE::String", "SomeAttribute,Hidden,Display=Extra Value" }),
                    },
                    // Return Type
                    new CE::ParameterType({ "", true, sizeof(CE::String), CE::FieldType::Plain, CE::FieldBaseType::String, "", ""})
                }));

            Functions.Add(new CE::FunctionType(CE::FunctionInitializer
                {
                    "ExposedFunc2", TEXT(Test::Child::TestClass::ExposedFunc2), false,
                    [&](void* instance, std::initializer_list<std::any> params, std::any& result) -> void {
                        Type* ptr = (Type*)instance;
                        ptr->ExposedFunc2(
                            std::any_cast<CE::String>(*(params.begin() + 0))
                        );
                    },
                    // Parameters
                    {
                        new CE::ParameterType({ "value", false, sizeof(CE::String), CE::FieldType::Plain, CE::FieldBaseType::String, "", "" }),
                    },
                    nullptr
                }));
        }
    };
    EXPORT_API extern CE_Generated_MetaClass_TestClass_Singleton& CE_Generated_MetaClass_TestClass_Instance; // End meta class

    // Cpp file
    EXPORT_API CE_Generated_MetaClass_TestClass_Singleton& CE_Generated_MetaClass_TestClass_Instance = CE_Generated_MetaClass_TestClass_Singleton::Get();

    CE_Generated_MetaClass_TestClass_Singleton::CE_Generated_MetaClass_TestClass_Singleton() 
        : CE::ClassType(CE::ClassInitializer{ _Name, sizeof(TestClass), _Type, _UnderlyingType, _ClassPath, _Attributes })
    {
        SetupProperties();
        SetupFunctions();
        ClassType::RegisterClassType(this);
    }

    CE_Generated_MetaClass_TestClass_Singleton::~CE_Generated_MetaClass_TestClass_Singleton()
    {
        ClassType::UnregisterClassType(this);
    }*/
}


int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    CE_LOG(Info, All, "Test string {} {} {} {}", 12.532f, "string", 'c', 1451);

    using namespace Test::Child;

    CE::ClassType* type = TestClass::Type();

    CE_LOG(Info, All, "Properties count: {}", type->GetPropertyCount());

    CE_LOG(Info, All, "Prop: {}", type->GetPropertyAt(0)->GetName());

    auto prop1 = type->GetPropertyAt(0);
    auto prop2 = type->GetPropertyAt(1);

    TestClass* testClass = new TestClass();
    testClass->Prop1 = "Old value";
    testClass->Prop2 = 412.215f;

    CE_LOG(Info, All, "Value: {} {}", prop1->GetValue<String>(testClass), prop2->GetValue<f32>(testClass));

    prop1->SetValue<String>(testClass, "New string");
    prop2->SetValue<f32>(testClass, 824.2124f);

    CE_LOG(Info, All, "Value: {} {}", testClass->Prop1, testClass->Prop2);

    //auto func1 = type->GetFunctionAt(0);
    //auto func2 = type->GetFunctionAt(1);

    //CE_LOG(Info, All, "Func name: {}", func1->GetName());

    //std::any result = func1->Invoke(testClass, { (s32)45, String("hello") });
    //CE::String resultString = std::any_cast<CE::String>(result);

    //CE_LOG(Info, All, "Result: {}", resultString);

    //func2->Invoke<void>(testClass, resultString);
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

