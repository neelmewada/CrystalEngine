#include "CoreRPI.h"

namespace CE::RPI
{

    RasterPass::RasterPass()
    {
	    
    }

    RasterPass::~RasterPass()
    {
	    
    }
	
    void RasterPass::ProduceScopes(FrameScheduler* scheduler)
    {
    	// TODO: Build scopes
    }
	
    void RasterPass::EmplaceAttachments(FrameAttachmentDatabase& attachmentDatabase)
    {
    	// TODO: Implement frame attachments
    	
    	auto emplaceAttachment = [&](const PassAttachmentBinding& attachmentBinding)
    		{
    			if (attachmentBinding.attachment != nullptr && attachmentBinding.attachment->lifetime == AttachmentLifetimeType::Transient)
    			{
    				const RPI::UnifiedAttachmentDescriptor& attachmentDescriptor = attachmentBinding.attachment->attachmentDescriptor;

    				switch (attachmentDescriptor.type)
    				{
    				case AttachmentType::Buffer:
    				{
    					RHI::BufferDescriptor bufferDescriptor{};
    					bufferDescriptor.name = attachmentBinding.attachment->name;
    					bufferDescriptor.bindFlags = attachmentDescriptor.bufferDesc.bindFlags;
    					bufferDescriptor.bufferSize = attachmentDescriptor.bufferDesc.byteSize;
    					bufferDescriptor.defaultHeapType = MemoryHeapType::Default;
    					
    					//attachmentDatabase.EmplaceFrameAttachment(bufferDescriptor.name, bufferDescriptor);
    				}
    					break;
    				case AttachmentType::Image:
    				{
    					RHI::ImageDescriptor imageDescriptor{};
    					imageDescriptor.name = attachmentBinding.attachment->name;
    					
    				}
    					break;
    				}
    			}
    		};
    	
    	for (const PassAttachmentBinding& attachmentBinding : inputBindings)
    	{
    		emplaceAttachment(attachmentBinding);
    	}

    	for (const PassAttachmentBinding& attachmentBinding : inputOutputBindings)
    	{
    		emplaceAttachment(attachmentBinding);
    	}

        for (const PassAttachmentBinding& attachmentBinding : outputBindings)
        {
            emplaceAttachment(attachmentBinding);
        }
    }

} // namespace CE::RPI
