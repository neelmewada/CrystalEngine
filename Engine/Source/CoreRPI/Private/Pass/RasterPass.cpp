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
	
    void RasterPass::EmplaceAttachments(FrameScheduler* scheduler)
    {
    	// TODO: Implement frame attachments
    	
    	auto emplaceAttachment = [&](const PassAttachmentBinding& attachmentBinding)
    		{
                FrameAttachmentDatabase& attachmentDatabase = scheduler->GetFrameAttachmentDatabase();

    			if (attachmentBinding.attachment != nullptr && attachmentBinding.attachment->lifetime == AttachmentLifetimeType::Transient)
    			{
                    if (attachmentDatabase.FindFrameAttachment(attachmentBinding.attachment->name) != nullptr)
                        return;

    				const RPI::UnifiedAttachmentDescriptor& attachmentDescriptor = attachmentBinding.attachment->attachmentDescriptor;
                    Ptr<PassAttachment> attachment = attachmentBinding.attachment;

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
    					imageDescriptor.name = attachment->name;
                        imageDescriptor.bindFlags = attachmentDescriptor.imageDesc.bindFlags;
                        imageDescriptor.defaultHeapType = MemoryHeapType::Default;
                        imageDescriptor.arrayLayers = attachmentDescriptor.imageDesc.arrayLayers;
                        imageDescriptor.dimension = attachmentDescriptor.imageDesc.dimension;
                        imageDescriptor.format = attachmentDescriptor.imageDesc.format;

                        if (attachment->sizeSource.IsFixedSize())
                        {
                            imageDescriptor.width = attachmentBinding.attachment->sizeSource.fixedSizes.x;
                            imageDescriptor.height = attachmentBinding.attachment->sizeSource.fixedSizes.y;
                            imageDescriptor.depth = attachmentBinding.attachment->sizeSource.fixedSizes.z;
                        }
                        else if (attachment->sizeSource.source.IsValid())
                        {
                            
                        }
                        else
                        {
                            return; // Invalid size entry
                        }

                        RHI::DeviceLimits* deviceLimits = gDynamicRHI->GetDeviceLimits();
                        if (!deviceLimits->IsFormatSupported(imageDescriptor.format, imageDescriptor.bindFlags))
                        {
                            Array<RHI::Format> fallbackFormats = attachmentBinding.attachment->fallbackFormats;

                            for (int i = 0; i < fallbackFormats.GetSize(); i++)
                            {
                                if (deviceLimits->IsFormatSupported(fallbackFormats[i], imageDescriptor.bindFlags))
                                {
                                    imageDescriptor.format = fallbackFormats[i];
                                    break;
                                }
                            }
                        }
                        
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
