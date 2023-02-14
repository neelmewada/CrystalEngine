
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

class ApplicationRequests
{
public:
    virtual void GetValue(int* outValue) = 0;
};

CE_EVENT_BUS(, ApplicationBus, ApplicationRequests);

class TestClass : public CE::Object, public ApplicationRequests
{
    CE_CLASS(TestClass, CE::Object)
public:

    TestClass() : Object("TestClass")
    {
        CE_CONNECT(ApplicationBus, this);
    }

    ~TestClass()
    {
        CE_DISCONNECT(ApplicationBus, this);
    }

    virtual void GetValue(int* outValue) override
    {
        LOG("GetValue() called");
        *outValue = 12;
    }
};

CE_RTTI_CLASS(, , TestClass,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        CE_FUNCTION(GetValue, Event)
    )
)

CE_RTTI_CLASS_IMPL(, , TestClass)

int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    CE_REGISTER_TYPES(TestClass);

    TestClass inst{};

    int val = 0;
    CE_PUBLISH(ApplicationBus, GetValue, &val);

    LOG("Value: " << val);
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

