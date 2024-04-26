#include "CoreRPI.h"

namespace CE::RPI
{

	RenderPipeline::RenderPipeline()
	{
		passTree = CreateObject<PassTree>(GetTransientPackage(MODULE_NAME), "PassTree");
		passTree->rootPass->renderPipeline = this;

		PassImageAttachmentDesc pipelineOutputDesc{};
		pipelineOutputDesc.name = "PipelineOutput";
		pipelineOutputDesc.lifetime = AttachmentLifetimeType::External;
		pipelineOutputDesc.imageDescriptor.dimension = Dimension::Dim2D;
		pipelineOutputDesc.imageDescriptor.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;

		auto pipelineOutput = new PassAttachment(pipelineOutputDesc);
		attachments.Add(pipelineOutput);
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

} // namespace CE::RPI
