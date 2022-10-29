
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

namespace Test::Child
{

    class TestClass : public CE::Object
    {
    public:
        CE_META_CLASS(TestClass, CE::Object);

        TestClass() : CE::Object()
        {

        }

    private:

        CE::String Prop1;

        CE::Array<CE::String> MyArray{};

        CE::Object* ObjRef = nullptr;

        String ExposedFunction(int integer)
        {
            return String("Value: ") + integer;
        }
    };

    struct PropertyInfo
    {
        const char* Name;
        u32 Offset;
        u32 Size;
        CE::FieldType Type;
        CE::FieldBaseType BaseType;
        const char* BaseTypePath;
        const char* Attributes = nullptr;
    };

    //struct CE_Generated_Meta_TestClass_Statics
    //{
    //public:
    //    static constexpr PropertyInfo Properties[] = {
    //        { "ObjRef", offsetof(TestClass, ObjRef), sizeof(TestClass::ObjRef), CE::FieldType::Pointer, CE::FieldBaseType::Object, "CE::Object", 
    //            "Max=12,ReadOnly"}, // attributes
    //    };
    //};

    //CE_META_BEGIN_CLASS(Test::Child, TestClass, CE::Object)
    //    CE_META_BEGIN_PROPERTIES(TestClass)
    //        CE_META_PROPERTY(Prop1, Plain, String, CE::String)
    //        CE_META_PROPERTY(MyArray, Array, String, CE::String)
    //        CE_META_PROPERTY(ObjRef, Pointer, Object, CE::Object, Hidden, Validate=Off)
    //    CE_META_END_PROPERTIES()
    //CE_META_END_CLASS();


}

int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    CE_LOG(Info, "Test string {} {} {} {}", 12.532f, "string", 'c', 1451);

    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

