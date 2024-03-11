#include "CoreRPI.h"

namespace CE::RPI
{
    GraphicsPipelineCollection::GraphicsPipelineCollection(const RHI::GraphicsPipelineDescriptor& desc)
        : desc(desc)
    {
        GraphicsPipelineVariant variant{};
        variant.sampleState = desc.multisampleState;
        
        u32 subpass = desc.subpass;

        if (desc.renderTarget != nullptr)
        {
            desc.renderTarget->GetAttachmentFormats(variant.colorFormats, variant.depthStencilFormat, subpass);
        }
        else
        {
            if (desc.rtLayout.subpasses.IsEmpty()) // Subpasses are empty => assume that there's only 1 subpass with all the attachments.
            {
                for (const auto& attachmentLayout : desc.rtLayout.attachmentLayouts)
                {
                    if (attachmentLayout.attachmentUsage == ScopeAttachmentUsage::Color)
                    {
                        variant.colorFormats.Add(attachmentLayout.format);
                    }
                    else if (attachmentLayout.attachmentUsage == ScopeAttachmentUsage::DepthStencil)
                    {
                        variant.depthStencilFormat = attachmentLayout.format;
                    }
                }
            }
            else
            {
                if (subpass < desc.rtLayout.subpasses.GetSize())
                {
                    RHI::RenderTargetSubpassLayout subpassLayout = desc.rtLayout.subpasses[subpass];
                    for (u32 attachmentIndex : subpassLayout.colorAttachments)
                    {
                        variant.colorFormats.Add(desc.rtLayout.attachmentLayouts[attachmentIndex].format);
                    }

                    if (subpassLayout.depthStencilAttachment.GetSize() > 0)
                    {
                        variant.depthStencilFormat = desc.rtLayout.attachmentLayouts[subpassLayout.depthStencilAttachment[0]].format;
                    }
                }
                else
                {
                    CE_LOG(Error, All, "GraphicsPipelineCollection Error: subpass index is outside the total subpasses!");
                }
            }
        }

        defaultVariant = variant;
        defaultPipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(desc);

        LockGuard<SharedMutex> lock{ mutex };
        pipelineCollection[variant] = defaultPipeline;
    }

    GraphicsPipelineCollection::~GraphicsPipelineCollection()
    {
        LockGuard<SharedMutex> lock{ mutex };

        for (auto renderTarget : renderTargets)
        {
            delete renderTarget;
        }
        renderTargets.Clear();

        for (const auto& [variant, pipeline] : pipelineCollection)
        {
            delete pipeline;
        }
        pipelineCollection.Clear();
    }

    RHI::PipelineState* GraphicsPipelineCollection::GetPipeline(const GraphicsPipelineVariant& variant)
    {
        mutex.Lock();

        if (pipelineCollection.KeyExists(variant))
        {
            auto result = pipelineCollection[variant];
            mutex.Unlock();
            return result;
        }

        mutex.Unlock();

        RHI::PipelineState* pipeline = nullptr;

        {
            RHI::GraphicsPipelineDescriptor desc = this->desc;
            desc.multisampleState.sampleCount = variant.sampleState.sampleCount;

            if (desc.renderTarget != nullptr)
            {
                RHI::RenderTarget* newRenderTarget = desc.renderTarget->Clone(variant.colorFormats, variant.depthStencilFormat, desc.subpass);
                desc.renderTarget = newRenderTarget;

                pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(desc);

                mutex.Lock();
                renderTargets.Add(newRenderTarget);
                pipelineCollection[variant] = pipeline;
                mutex.Unlock();
            }
            else
            {
                if (desc.rtLayout.subpasses.IsEmpty()) // Subpasses are empty => assume that there's only 1 subpass with all the attachments.
                {
                    int colorIdx = 0;
                    for (auto& attachmentLayout : desc.rtLayout.attachmentLayouts)
                    {
                        attachmentLayout.multisampleState = desc.multisampleState;

                        if (attachmentLayout.attachmentUsage == ScopeAttachmentUsage::Color && colorIdx < variant.colorFormats.GetSize())
                        {
                            RHI::Format format = variant.colorFormats[colorIdx++];
                            if (format != RHI::Format::Undefined)
                                attachmentLayout.format = format;
                        }
                        else if (attachmentLayout.attachmentUsage == ScopeAttachmentUsage::DepthStencil && variant.depthStencilFormat != RHI::Format::Undefined)
                        {
                            attachmentLayout.format = variant.depthStencilFormat;
                        }
                    }
                }
                else
                {
                    if (desc.subpass < desc.rtLayout.subpasses.GetSize())
                    {
                        RHI::RenderTargetSubpassLayout subpassLayout = desc.rtLayout.subpasses[desc.subpass];
                        int colorIdx = 0;
                        for (u32 attachmentIndex : subpassLayout.colorAttachments)
                        {
                            if (colorIdx >= variant.colorFormats.GetSize())
                                break;
                            RHI::Format format = variant.colorFormats[colorIdx++];
                            if (format != RHI::Format::Undefined)
                                desc.rtLayout.attachmentLayouts[attachmentIndex].format = format;
                        }

                        if (subpassLayout.depthStencilAttachment.GetSize() > 0 && variant.depthStencilFormat != RHI::Format::Undefined)
                        {
                            desc.rtLayout.attachmentLayouts[subpassLayout.depthStencilAttachment[0]].format = variant.depthStencilFormat;
                        }
                    }
                    else
                    {
                        CE_LOG(Error, All, "GraphicsPipelineCollection Error: subpass index is outside the total subpasses!");
                        return nullptr;
                    }
                }

                pipeline = RHI::gDynamicRHI->CreateGraphicsPipeline(desc);

                mutex.Lock();
                pipelineCollection[variant] = pipeline;
                mutex.Unlock();
            }
        }

        return pipeline;
    }

} // namespace CE::RPI
