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
        
		CompilePipeline();
	}

	bool RenderPipeline::IsRootPass(const Name& passName)
	{
		return passTree->rootPass != nullptr && passTree->rootPass->GetName() == passName;
	}

	bool RenderPipeline::CompilePipeline()
	{
        passTree->Clear();
        
		const RPI::PassRequest& rootPassRequest = descriptor.rootPass;

		// Instantiate passes recursively
		Pass* rootPass = InstantiatePassesRecursively(rootPassRequest, nullptr);
		if (rootPass == nullptr)
			return false;

		// Connect pass slots & attachments
		BuildPassConnectionsRecursively(rootPassRequest);

		return true;
	}

	void RenderPipeline::SetupRootPass(ParentPass* rootPass)
	{
		const RPI::PassRequest& rootPassRequest = descriptor.rootPass;
		int index = descriptor.passDefinitions.IndexOf([&](const PassDefinition& x) { return x.name == rootPassRequest.passDefinition; });
		if (index < 0)
			return;

		passTree->rootPass = rootPass;

		const RPI::PassDefinition& rootPassDef = descriptor.passDefinitions[index];
		int slotIdx = -1; // Find the first InputOutput slot & create an attachment out of it.
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
		imageAttachmentDesc.imageDescriptor.format = RHI::Format::R8G8B8A8_UNORM; // RGBA or BGRA
		imageAttachmentDesc.imageDescriptor.dimension = Dimension::Dim2D;
		imageAttachmentDesc.generateFullMipChain = false;
		imageAttachmentDesc.fallbackFormats = { RHI::Format::B8G8R8A8_UNORM };

		Ptr<PassAttachment> pipelineOutputAttachment = new PassAttachment(imageAttachmentDesc);
		rootPass->passAttachments.Add(pipelineOutputAttachment);
	}

	void RenderPipeline::BuildPassConnectionsRecursively(const PassRequest& passRequest)
	{
		int index = descriptor.passDefinitions.IndexOf([&](const PassDefinition& x) { return x.name == passRequest.passDefinition; });
		if (index < 0)
			return;

		const PassDefinition& passDefinition = descriptor.passDefinitions[index];
		Pass* pass = passTree->FindPass(passRequest.passName);
		if (pass == nullptr)
			return;

		// Connect slots to other slots.
		for (const auto& connection : passRequest.connections)
		{
			// The local slot descriptor
			PassSlot* localSlot = passDefinition.FindSlot(connection.localSlot);
			if (localSlot == nullptr)
				continue;

			Name passPath = connection.attachmentRef.pass; // Specials: $this, $parent, $root
			Name attachmentName = connection.attachmentRef.attachment;

			// The pass instance we are connecting to.
			Pass* attachmentPass = passTree->GetPassAtPath(passPath, pass);
			if (!attachmentPass)
				continue;

			// The definition of the pass we are connecting to.
			PassDefinition* attachmentPassDef = descriptor.FindPassDefinitionForPassRequest(attachmentPass->GetName());
			if (!attachmentPassDef)
				continue;

			PassAttachmentBinding* attachmentPassBinding = attachmentPass->FindBinding(attachmentName);
            PassAttachmentBinding* thisPassBinding = pass->FindBinding(connection.localSlot);
            
            if (attachmentPassBinding == nullptr || thisPassBinding == nullptr)
                continue;
            
            thisPassBinding->connectedBinding = attachmentPassBinding;
		}
        
        for (const PassRequest& child : passRequest.childPasses)
        {
            BuildPassConnectionsRecursively(child);
        }
	}

	Pass* RenderPipeline::InstantiatePassesRecursively(const PassRequest& passRequest, ParentPass* parentPass)
	{
		Object* outer = parentPass;
		if (parentPass == nullptr) // True if root pass
			outer = passTree;

		PassDefinition* passDefinition = descriptor.FindPassDefinition(passRequest.passDefinition);
		if (passDefinition == nullptr)
			return nullptr;

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
        
        // Create PassAttachmentBinding's for each slot
        for (const PassSlot& slot : passDefinition->slots)
        {
            PassAttachmentBinding passAttachmentBinding{};
            passAttachmentBinding.name = slot.name;
            passAttachmentBinding.attachmentUsage = slot.attachmentUsage;
            passAttachmentBinding.ownerPass = pass;
            passAttachmentBinding.slotType = slot.slotType;
            
            switch (slot.slotType) 
            {
            case PassSlotType::Input:
                pass->inputBindings.Add(passAttachmentBinding);
                break;
            case PassSlotType::Output:
                pass->outputBindings.Add(passAttachmentBinding);
                break;
            case PassSlotType::InputOutput:
                pass->inputOutputBindings.Add(passAttachmentBinding);
                break;
            }
        }

		// Build connections defined in the PassDefinition. Connections in PassDefinition should always
		// be for connecting local slots to actual attachments.
		for (const auto& connection : passDefinition->connections)
		{
			PassSlot* localSlot = passDefinition->FindSlot(connection.localSlot);
			if (!localSlot)
				continue;

			Pass* attachmentPass = passTree->GetPassAtPath(connection.attachmentRef.pass, pass);
			if (!attachmentPass)
				continue;
			
			for (int i = 0; i < attachmentPass->passAttachments.GetSize(); i++)
			{
				PassAttachment* attachment = attachmentPass->passAttachments[i];
				// Found an attachment with matching name
				if (attachment->name == connection.attachmentRef.attachment)
				{
					// Infer attachment usage, etc from the attachment
					if (attachment->attachmentDescriptor.type == AttachmentType::Image)
					{
						const ImageDescriptor& imageDesc = attachment->attachmentDescriptor.imageDesc;
						if (EnumHasFlag(imageDesc.bindFlags, RHI::TextureBindFlags::DepthStencil))
						{
							localSlot->attachmentUsage = RHI::ScopeAttachmentUsage::DepthStencil;
						}
						else if (EnumHasFlag(imageDesc.bindFlags, RHI::TextureBindFlags::Color))
						{
							localSlot->attachmentUsage = RHI::ScopeAttachmentUsage::RenderTarget;
						}
						else if (EnumHasFlag(imageDesc.bindFlags, RHI::TextureBindFlags::SubpassInput))
						{
							localSlot->attachmentUsage = RHI::ScopeAttachmentUsage::SubpassInput;
						}
						else if (EnumHasAnyFlags(imageDesc.bindFlags, RHI::TextureBindFlags::ShaderRead | RHI::TextureBindFlags::ShaderWrite))
						{
							localSlot->attachmentUsage = RHI::ScopeAttachmentUsage::Shader;
						}
					}
                    
                    PassAttachmentBinding* attachmentBinding = pass->FindBinding(localSlot->name);
					attachmentBinding->attachment = attachment;

					break;
				}
			}
		}

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
