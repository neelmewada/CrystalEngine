
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

#define EXPORT_API


namespace Test::Child
{
    class Some0 : public CE::Object
    {
        CE_CLASS(Some0, CE::Object);
    public:
        Some0() : CE::Object("Some0 Object")
        {}
        int s0 = 0;
        CE::String baseString{};

    public:
        virtual void PrintHello() const
        {
            CE_LOG(Info, All, "Hello world from Some0");
        }

        void DoTest(s32 number, String text)
        {
            CE_LOG(Info, All, "Test called: {} | {}", number, text);
        }

        f32 Add(f32 a, f32 b) const
        {
            CE_LOG(Info, All, "Add called: {} + {} = {}", a, b, a + b);
            return a + b;
        }
    };

    class BaseClass : public Some0
    {
        CE_CLASS(BaseClass, Some0);
    public:
        BaseClass() : Some0()
        {}

        CE::String childString{};

        virtual void PrintHello() const override
        {
            CE_LOG(Info, All, "Hello world from BaseClass");
        }
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
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Abstract),
    CE_FIELD_LIST(
        CE_FIELD(s0)
        CE_FIELD(baseString, TextArea, MaxLength = 32)
    ),
    CE_FUNCTION_LIST(
        CE_FUNCTION(DoTest, HiddenFunction)
        CE_FUNCTION(Add)
    )
);

CE_RTTI_CLASS_IMPL(,Test::Child, Some0);


CE_RTTI_CLASS(,Test::Child, BaseClass,
    CE_SUPER(Test::Child::Some0),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(childString)
    ),
    CE_FUNCTION_LIST()
);

CE_RTTI_CLASS_IMPL(,Test::Child, BaseClass);

class MyCustomRequests : public CE::IBusInterface
{
    CE_CLASS(MyCustomRequests, CE::IBusInterface);

public:

    virtual void PrintOut(int number) = 0;

};

CE_RTTI_CLASS(, , MyCustomRequests,
    CE_SUPER(),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(

    ),
    CE_FUNCTION_LIST()
);

CE_RTTI_CLASS_IMPL(, , MyCustomRequests);

using CustomRequestBus = CE::MessageBus<MyCustomRequests>;

class SomeTestClass : public CustomRequestBus::Handler
{
    CE_CLASS(SomeTestClass, CustomRequestBus::Handler);

public:

    virtual SIZE_T GetAddress() override { return 1; }

    virtual void PrintOut(int number) override { std::cout << "Print: " << number << std::endl; }

};

CE_RTTI_CLASS(, , SomeTestClass,
    CE_SUPER(),
    false,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    ),
    CE_FUNCTION_LIST()
);

CE_RTTI_CLASS_IMPL(, , SomeTestClass);

int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    using namespace Test::Child;

    CE_REGISTER_TYPES(Some0, BaseClass);

    Matrix4x4 mat = Matrix4x4({
        { 5, -2,  2, 7 },
        { 1,  0,  0, 3 },
        { -3, 1,  5, 0 },
        { 3, -1, -9, 4 }
    });

    Matrix4x4 inverse = mat.GetInverse();

    CE_LOG(Info, All, "Matrix: \n{}", (inverse));
    
    CE_LOG(Info, All, "Array: {}", TYPEID(Array<f32>));
    CE_LOG(Info, All, "Array: {}", TYPEID(Array<u8>));
    CE_LOG(Info, All, "Array: {}", TYPEID(Array<Vec4>));
    
    CE_LOG(Info, All, "Some0: {}", TYPEID(Test::Child::Some0));
    CE_LOG(Info, All, "Base Class: {}", TYPEID(Test::Child::BaseClass));
    
    CE_LOG(Info, All, "Registered: {}", TypeInfo::GetRegisteredCount());
    
    auto type = GetTypeInfo(TYPEID(Test::Child::Some0));
    
    if (type != nullptr)
    {
        auto name = type->GetName();

        CE_LOG(Info, All, "Type found: {}", name);

        CE_LOG(Info, All, "Names {}: {} ; {} ; {}", name.GetComponentCount(), name.GetComponentAt(0), name.GetComponentAt(1), name.GetComponentAt(2));

        auto classType = (ClassType*)type;
        auto field = classType->GetFirstField();

        Some0 instance{};

        CE_LOG(Info, All, "Local functions: {}", classType->GetLocalFunctionCount());

        for (int i = 0; i < classType->GetLocalFunctionCount(); i++)
        {
            auto funcType = classType->GetLocalFunctionAt(i);

            CE_LOG(Info, All, "Function {}: {}", i, funcType->GetName());

            if (i == 0)
            {
                funcType->Invoke(&instance, { (s32)214, String("Hello") });
            }
            else if (i == 1)
            {
                CE::Variant returnValue = funcType->Invoke(&instance, { (f32)12.5f, (f32)0.5f });

                if (returnValue.HasValue() && returnValue.IsOfType<f32>())
                {
                    CE_LOG(Info, All, "Add Result: {}", returnValue.GetValue<f32>());
                }
            }
        }

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

    CE_LOG(Info, All, "Dynamic Casting...");

    BaseClass* ptr = new BaseClass;
    ptr->baseString = "this is base";
    ptr->childString = "this is child";
    ptr->s0 = -24;

    SerializedObject obj{ ptr->GetType() };

    CE::IO::MemoryStream memStream{};
    obj.Serialize(ptr, &memStream);

    CE_LOG(Info, All, "Serialized Data:\n{}", memStream.GetBuffer());

    memStream.Free();

    {
        auto classType = (ClassType*)ptr->GetType();
        auto cast = classType->TryCast((CE::IntPtr)ptr, TYPEID(Some0));
        
        CE_LOG(Info, All, "Cast: {} | {} ; {}", cast == 0 ? "Failed" : "Succeeded", cast, (IntPtr)ptr);
    }

    CE_DELETE(ptr);

    SomeTestClass handler{};

    CustomRequestBus::BusConnect(&handler);

    CustomRequestBus::Broadcast(&MyCustomRequests::PrintOut, 142);

    CustomRequestBus::BusDisconnect(&handler);
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

