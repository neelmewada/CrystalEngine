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
        Name scopeId = GetScopeId();
        
        scheduler->BeginScope(scopeId);
        {
            // - Use Attachments -
            
            auto useAttachment = [&](const PassAttachmentBinding& attachmentBinding)
                {
                    Ptr<PassAttachment> attachment = attachmentBinding.GetOriginalAttachment();
                    if (attachment == nullptr)
                        return;

                    const PassSlot* slot = nullptr;

					for (const PassSlot& passSlot : slots)
					{
						if (passSlot.slotType == attachmentBinding.slotType &&
                            passSlot.name == attachmentBinding.name)
						{
                            slot = &passSlot;
							break;
						}
					}

					if (!slot)
                        return;

                    ScopeAttachmentAccess attachmentAccess = ScopeAttachmentAccess::Undefined;
                    switch (slot->slotType)
                    {
                    case PassSlotType::Input:
                        attachmentAccess = ScopeAttachmentAccess::Read;
	                    break;
                    case PassSlotType::Output:
                        attachmentAccess = ScopeAttachmentAccess::Write;
	                    break;
                    case PassSlotType::InputOutput:
                        attachmentAccess = ScopeAttachmentAccess::ReadWrite;
	                    break;
                    default:
                        return;
                    }

                    if (attachment->attachmentDescriptor.type == AttachmentType::Image)
                    {
                        ImageScopeAttachmentDescriptor imageScopeAttachment{};
                        imageScopeAttachment.attachmentId = attachment->attachmentId;
                        imageScopeAttachment.loadStoreAction = slot->loadStoreAction;
                        imageScopeAttachment.multisampleState.sampleCount = attachment->attachmentDescriptor.imageDesc.sampleCount;

                        scheduler->UseAttachment(imageScopeAttachment, slot->attachmentUsage, attachmentAccess);
                    }
                    else if (attachment->attachmentDescriptor.type == AttachmentType::Buffer)
                    {
                        BufferScopeAttachmentDescriptor bufferScopeAttachment{};
                        bufferScopeAttachment.attachmentId = attachment->attachmentId;
                        bufferScopeAttachment.loadStoreAction = slot->loadStoreAction;

                        scheduler->UseAttachment(bufferScopeAttachment, slot->attachmentUsage, attachmentAccess);
                    }
                };

            for (const PassAttachmentBinding& attachmentBinding : inputBindings)
            {
                useAttachment(attachmentBinding);
            }

            for (const PassAttachmentBinding& attachmentBinding : inputOutputBindings)
            {
                useAttachment(attachmentBinding);
            }

            for (const PassAttachmentBinding& attachmentBinding : outputBindings)
            {
                useAttachment(attachmentBinding);
            }

            // - Use SRGs -

            if (shaderResourceGroup)
            {
	            scheduler->UseShaderResourceGroup(shaderResourceGroup);
            }
            
        }
        scheduler->EndScope();
    }
	
    void RasterPass::EmplaceAttachments(FrameScheduler* scheduler)
    {
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
    					
    					//attachmentDatabase.EmplaceFrameAttachment(attachment->attachmentId, bufferDescriptor);
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
                        imageDescriptor.sampleCount = attachmentDescriptor.imageDesc.sampleCount;
                        imageDescriptor.mipLevels = attachmentDescriptor.imageDesc.mipCount;

                        if (attachment->sizeSource.IsFixedSize())
                        {
                            imageDescriptor.width = attachment->sizeSource.fixedSizes.x;
                            imageDescriptor.height = attachment->sizeSource.fixedSizes.y;
                            imageDescriptor.depth = attachment->sizeSource.fixedSizes.z;
                        }
                        else if (attachment->sizeSource.source.IsValid())
                        {
                            Ptr<PassAttachment> sourcePassAttachment = renderPipeline->FindAttachment(attachment->sizeSource.source);
                            if (!sourcePassAttachment)
                                return;

                            FrameAttachment* sourceAttachment = attachmentDatabase.FindFrameAttachment(sourcePassAttachment->attachmentId);
                            if (!sourceAttachment)
                                return;

                            if (sourceAttachment->IsSwapChainAttachment())
                            {
                                auto srcSwapChainAttachment = (SwapChainFrameAttachment*)sourceAttachment;
                                imageDescriptor.width = srcSwapChainAttachment->GetSwapChain()->GetWidth();
                                imageDescriptor.height = srcSwapChainAttachment->GetSwapChain()->GetHeight();
                                imageDescriptor.depth = 1;

                                imageDescriptor.width *= attachment->sizeSource.sizeMultipliers.x;
                                imageDescriptor.height *= attachment->sizeSource.sizeMultipliers.y;
                            }
                            else if (sourceAttachment->IsImageAttachment())
                            {
                                auto srcImageAttachment = (ImageFrameAttachment*)sourceAttachment;
                                imageDescriptor.width = srcImageAttachment->GetImageDescriptor().width;
                                imageDescriptor.height = srcImageAttachment->GetImageDescriptor().height;
                                imageDescriptor.depth = srcImageAttachment->GetImageDescriptor().depth;

                                imageDescriptor.width *= attachment->sizeSource.sizeMultipliers.x;
                                imageDescriptor.height *= attachment->sizeSource.sizeMultipliers.y;
                                imageDescriptor.depth *= attachment->sizeSource.sizeMultipliers.z;
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
                        
                        attachmentDatabase.EmplaceFrameAttachment(attachment->attachmentId, imageDescriptor);
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
