#include "CoreRPI.h"

namespace CE::RPI
{

	RenderPipeline::RenderPipeline()
	{
		passTree = CreateObject<PassTree>(GetTransientPackage(MODULE_NAME), "PassTree", OF_Transient);
	}

    RenderPipeline::~RenderPipeline()
    {
		if (passTree != nullptr)
		{
			passTree->Destroy();
			passTree = nullptr;
		}
    }

	RenderPipeline* RenderPipeline::Create(const RenderPipelineDescriptor& descriptor, Scene* ownerScene)
	{
		RenderPipeline* pipeline = new RenderPipeline();

		// Setup
		pipeline->descriptor = descriptor;
		pipeline->scene = ownerScene;
		pipeline->InitializeInternal();

		return pipeline;
	}

	RenderPipeline* RenderPipeline::CreateFromJson(const String& jsonString, Scene* ownerScene)
	{
		MemoryStream stream = MemoryStream(jsonString.GetData(), jsonString.GetLength());
		stream.SetBinaryMode(true);
		return CreateFromJson(&stream, ownerScene);
	}

	RenderPipeline* RenderPipeline::CreateFromJson(Stream* json, Scene* ownerScene)
	{
		RenderPipeline* pipeline = new RenderPipeline();

		JsonFieldDeserializer deserializer = JsonFieldDeserializer( GetStaticStruct<RenderPipelineDescriptor>(), &pipeline->descriptor );
		deserializer.Deserialize(json);

		// Setup
		pipeline->scene = ownerScene;
		pipeline->InitializeInternal();

		return pipeline;
	}

	void RenderPipeline::InitializeInternal()
	{
		renderTarget = descriptor.renderTarget;
        
		name = descriptor.name;
		rootPassDefinitionName = descriptor.rootPass.passDefinition;
		mainViewTag = descriptor.mainViewTag;
        
	}

	void RenderPipeline::CompileTree()
	{
        passTree->Clear();
        
		passTree->rootPass = CreateObject<ParentPass>(passTree, "RootPass");

		
	}

	Pass* RenderPipeline::CreatePassFromDefinition(const PassDefinition& passDefinition)
	{
		if (!passDefinition.name.IsValid() || !passDefinition.passClass.IsValid())
			return nullptr;

		ClassType* passClassType = PassRegistry::Get().GetPassClass(passDefinition.passClass);
		if (passClassType == nullptr)
			return nullptr;

		Pass* pass = CreateObject<Pass>(passTree, passDefinition.name.GetString(), OF_NoFlags, passClassType);
		if (pass == nullptr)
			return nullptr;

		if (passDefinition.passData.drawListTag.IsValid())
		{
			pass->drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(passDefinition.passData.drawListTag);
		}

		if (passDefinition.passData.viewTag.IsValid())
		{
			pass->pipelineViewTag = passDefinition.passData.viewTag;
		}
	}

} // namespace CE::RPI
