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
        
		CompileTree();
	}

	bool RenderPipeline::IsRootPass(const Name& passName)
	{
		return passTree->rootPass != nullptr && passTree->rootPass->GetName() == passName;
	}

	bool RenderPipeline::CompileTree()
	{
        passTree->Clear();
        
		const RPI::PassRequest& rootPassRequest = descriptor.rootPass;

		// Instantiate passes recursively
		Pass* rootPass = InstantiatePassesRecursively(rootPassRequest, nullptr);
		if (rootPass == nullptr)
			return false;

		// Connect pass slots & attachments
		

		return true;
	}

	void RenderPipeline::SetupRootPass(ParentPass* rootPass)
	{
		const RPI::PassRequest& rootPassRequest = descriptor.rootPass;
		int index = descriptor.passDefinitions.IndexOf([&](const PassDefinition& x) { return x.name == rootPassRequest.passDefinition; });
		if (index < 0)
			return;

		const RPI::PassDefinition& rootPassDef = descriptor.passDefinitions[index];
		int slotIdx = -1;
		for (int i = 0; i < rootPassDef.slots.GetSize(); i++)
		{
			if (rootPassDef.slots[i].slotType == RPI::PassSlotType::InputOutput)
			{
				slotIdx = i;
				break;
			}
		}

		if (slotIdx < 0)
			return;

		PassImageAttachmentDesc imageAttachmentDesc{};
		imageAttachmentDesc.name = rootPassDef.slots[slotIdx].name;
		imageAttachmentDesc.lifetime = RHI::AttachmentLifetimeType::External;
		imageAttachmentDesc.imageDescriptor.bindFlags = RHI::TextureBindFlags::Color;
		imageAttachmentDesc.imageDescriptor.arraySize = 1;
		imageAttachmentDesc.imageDescriptor.mipCount = 1;
		imageAttachmentDesc.imageDescriptor.format = RHI::Format::R8G8B8A8_UNORM;
		imageAttachmentDesc.imageDescriptor.dimension = Dimension::Dim2D;
		imageAttachmentDesc.generateFullMipChain = false;
		imageAttachmentDesc.fallbackFormats = { RHI::Format::B8G8R8A8_UNORM };

		Ptr<PassAttachment> pipelineOutputAttachment = new PassAttachment(imageAttachmentDesc);
		rootPass->passAttachments.Add(pipelineOutputAttachment);
	}

	void RenderPipeline::BuildPassConnections(const PassRequest& passRequest)
	{
		int index = descriptor.passDefinitions.IndexOf([&](const PassDefinition& x) { return x.name == passRequest.passDefinition; });
		if (index < 0)
			return;

		const PassDefinition& passDefinition = descriptor.passDefinitions[index];
		Pass* pass = passTree->FindPass(passRequest.passName);
		if (pass == nullptr)
			return;

		// Pass definition connections are only made to connect local slots to actual attachments, NOT to other slots.
		for (const auto& connection : passDefinition.connections)
		{
			PassSlot* localSlot = passDefinition.FindSlot(connection.localSlot);
			if (localSlot == nullptr)
				continue;

			Name passPath = connection.attachmentRef.pass; // Specials: $this, $parent, $root
			Name attachmentName = connection.attachmentRef.attachment;

			Pass* targetPass = passTree->GetPassAtPath(passPath, pass);
			if (targetPass == nullptr)
				continue;


		}
		
	}

	Pass* RenderPipeline::InstantiatePassesRecursively(const PassRequest& passRequest, ParentPass* parentPass)
	{
		Object* outer = parentPass;
		if (parentPass == nullptr) // True if root pas
			outer = passTree;

		PassDefinition* passDefinition = descriptor.FindPassDefinition(passRequest.passDefinition);
		if (passDefinition == nullptr)
			return;

		if (!passDefinition->name.IsValid() || !passDefinition->passClass.IsValid())
			return nullptr;

		ClassType* passClassType = PassRegistry::Get().GetPassClass(passDefinition->passClass);
		if (passClassType == nullptr)
			return nullptr;
		if (parentPass == nullptr && !passClassType->IsSubclassOf<ParentPass>()) // Root pass should always be subclass of ParentPass class
			return nullptr;
		
		Pass* pass = CreateObject<Pass>(outer, passRequest.passName.GetString(), OF_NoFlags, passClassType);
		if (pass == nullptr)
			return nullptr;

		pass->parentPass = parentPass;

		if (passDefinition->passData.drawListTag.IsValid())
		{
			pass->drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(passDefinition->passData.drawListTag);
		}

		if (passDefinition->passData.viewTag.IsValid())
		{
			pass->pipelineViewTag = passDefinition->passData.viewTag;
		}

		// Attachments owned by this pass
		for (PassImageAttachmentDesc& attachmentDesc : passDefinition->imageAttachments)
		{
			pass->passAttachments.Add(new PassAttachment(attachmentDesc));
		}
		for (PassBufferAttachmentDesc& attachmentDesc : passDefinition->bufferAttachments)
		{
			pass->passAttachments.Add(new PassAttachment(attachmentDesc));
		}

		if (parentPass == nullptr)
			SetupRootPass((ParentPass*)pass);

		if (pass->IsParentPass())
		{
			// Child passes
			for (const PassRequest& childPassRequest : passRequest.childPasses)
			{
				ParentPass* cast = (ParentPass*)pass;
				Pass* childPass = InstantiatePassesRecursively(childPassRequest, cast);
				if (childPass != nullptr)
				{
					cast->AddChild(childPass);
				}
			}
		}

		return pass;
	}

} // namespace CE::RPI
