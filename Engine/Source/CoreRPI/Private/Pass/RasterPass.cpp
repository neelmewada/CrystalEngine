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
    					bufferDescriptor.name = attachment->name;
    					bufferDescriptor.bindFlags = attachmentDescriptor.bufferDesc.bindFlags;
    					bufferDescriptor.bufferSize = attachmentDescriptor.bufferDesc.byteSize;
    					bufferDescriptor.defaultHeapType = MemoryHeapType::Default;
    					
    					//attachmentDatabase.EmplaceFrameAttachment(attachment->name, bufferDescriptor);
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
                            imageDescriptor.width = attachment->sizeSource.fixedSizes.x;
                            imageDescriptor.height = attachment->sizeSource.fixedSizes.y;
                            imageDescriptor.depth = attachment->sizeSource.fixedSizes.z;
                        }
                        else if (attachment->sizeSource.source.IsValid())
                        {
                            FrameAttachment* sourceAttachment = attachmentDatabase.FindFrameAttachment(attachment->sizeSource.source);
                            if (!sourceAttachment)
                                return;
                            if (sourceAttachment->IsImageAttachment())
                            {
                                auto srcImageAttachment = (ImageFrameAttachment*)sourceAttachment;
                                imageDescriptor.width = srcImageAttachment->GetImageDescriptor().width;
                                imageDescriptor.height = srcImageAttachment->GetImageDescriptor().height;
                                imageDescriptor.depth = srcImageAttachment->GetImageDescriptor().depth;

                                imageDescriptor.width *= attachment->sizeSource.sizeMultipliers.x;
                                imageDescriptor.height *= attachment->sizeSource.sizeMultipliers.y;
                                imageDescriptor.depth *= attachment->sizeSource.sizeMultipliers.z;
                            }
                            else if (sourceAttachment->IsSwapChainAttachment())
                            {
                                auto srcSwapChainAttachment = (SwapChainFrameAttachment*)sourceAttachment;
                                imageDescriptor.width = srcSwapChainAttachment->GetSwapChain()->GetWidth();
                                imageDescriptor.height = srcSwapChainAttachment->GetSwapChain()->GetHeight();
                                imageDescriptor.depth = 1;

                                imageDescriptor.width *= attachment->sizeSource.sizeMultipliers.x;
                                imageDescriptor.height *= attachment->sizeSource.sizeMultipliers.y;
                            }
                            else
                            {
                                return;
                            }
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
                        
                        //attachmentDatabase.EmplaceFrameAttachment(attachment->name, imageDescriptor);
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
