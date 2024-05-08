#include "CoreRPI.h"

namespace CE::RPI
{

	RenderPipeline::RenderPipeline()
	{
		passTree = CreateObject<PassTree>(GetTransientPackage(MODULE_NAME), "PassTree");
		passTree->rootPass->renderPipeline = this;
	}

    RenderPipeline::~RenderPipeline()
    {
		if (passTree)
		{
			passTree->Destroy();
			passTree = nullptr;
		}
    }

    Ptr<PassAttachment> RenderPipeline::FindAttachment(Name name)
    {
		for (PassAttachment* passAttachment : attachments)
		{
			if (passAttachment->name == name)
			{
				return passAttachment;
			}
		}
		return nullptr;
    }

    PassAttachment* RenderPipeline::AddAttachment(const RPI::PassImageAttachmentDesc& imageDesc)
    {
		auto attachment = new PassAttachment(imageDesc);
		attachments.Add(attachment);
		return attachment;
    }

    PassAttachment* RenderPipeline::AddAttachment(const RPI::PassBufferAttachmentDesc& bufferDesc)
    {
		auto attachment = new PassAttachment(bufferDesc);
		attachments.Add(attachment);
		return attachment;
    }
	
    void RenderPipeline::ImportScopeProducers(RHI::FrameScheduler* scheduler)
    {
		std::function<void(Pass*)> addScopesRecursively = [&](Pass* currentPass)
			{
				if (currentPass == nullptr)
					return;
				
				if (currentPass->IsParentPass())
				{
					auto parentPass = (ParentPass*)currentPass;

					for (Pass* pass : parentPass->passes)
					{
						addScopesRecursively(pass);
					}
					return;
				}

				scheduler->AddScopeProducer(currentPass);
			};

		addScopesRecursively(passTree->rootPass);

		const auto& scopeProducers = scheduler->GetScopeProducers();

		for (int i = 0; i < scopeProducers.GetSize(); i++)
		{
			IScopeProducer* scopeProducer = scopeProducers[i];
			scopeProducer->EmplaceAttachments(scheduler);
		}

		for (int i = 0; i < scopeProducers.GetSize(); i++)
		{
			IScopeProducer* scopeProducer = scopeProducers[i];
			scopeProducer->ProduceScopes(scheduler);
		}
    }

} // namespace CE::RPI
