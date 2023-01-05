
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl

using namespace CE;

struct TestConfig
{
    CE_STRUCT(TestConfig)

public:
    int intValue = 0;
    String stringValue{};
};

CE_RTTI_STRUCT(,,TestConfig,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(intValue)
        CE_FIELD(stringValue, Category = "Some Category")
    )
)

CE_RTTI_STRUCT_IMPL(,,TestConfig)


int main(int argc, char* argv[])
{
    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    CE_REGISTER_TYPES(TestConfig);

    TestConfig test{};

    auto type = TestConfig::Type();
    auto field = type->FindFieldWithName("stringValue");
    const auto& attrs = field->GetAttributes();

    CE_LOG(Info, All, "Count: {}", attrs.GetSize());
    
    for (int i = 0; i < attrs.GetSize(); i++)
    {
        const Attribute& attrib = attrs[i];
        CE_LOG(Info, All, "{} = {}", attrib.GetKey(), attrib.GetValue());
    }
    
    CE::ConfigParser parser = CE::ConfigParser(type);
    
    parser.Parse(&test, "TestConfig.ini");
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}

