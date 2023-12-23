
#include "CoreRPI.h"
#include <gtest/gtest.h>

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static void TestBegin()
{
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
}

static void TestEnd()
{
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("Core");
}

const char* DescriptorParsing_Basic_JSON = R"(
{
    "Name": "DefaultPipeline",
    "MainViewTag": "Camera",
    // Root pass (PassRequest)
    "RootPass": {
        "PassName": "DefaultPipelineRoot",
        "PassDefinition": "DefaultPipelineRoot",
        "ChildPasses": [ // [PassRequest] list
            
        ]
    },
    "PassDefinitions": [
        // Root Pass (DefaultPipelineRoot): The render target image attachment is automaticall inferred, no need to manually define it.
        {
            "Name": "DefaultPipelineRoot",
            "PassClass": "ParentPass",
            "Slots": [
                // Final color output
                {
                    "Name": "PipelineOutput",
                    "SlotType": "InputOutput"
                }
            ]
        }
    ]
}
)";

TEST(RenderPipeline, DescriptorParsing_Basic)
{
	TEST_BEGIN;

	RenderPipelineDescriptor desc{};

	JsonFieldDeserializer deserializer{ desc.GetStruct(), &desc };

	JValue root{};
	JsonSerializer::Deserialize2(DescriptorParsing_Basic_JSON, root);
	
	while (deserializer.HasNext())
	{
		deserializer.ReadNext(root);
	}

	TEST_END;
}
