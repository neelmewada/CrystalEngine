
#include "VulkanRHIPrivate.h"

#include "VulkanRenderPass.h"
#include "VulkanViewport.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"

namespace CE
{

    // ****************************************************
    // VulkanRenderTargetLayout

    VulkanRenderTargetLayout::VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHI::RenderTargetLayout& rtLayout)
    {
        using namespace CE::RHI;

        this->width = width;
        this->height = height;
        this->backBufferCount = rtLayout.backBufferCount;
        this->simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;
        this->presentationRTIndex = rtLayout.presentationRTIndex;
        isValid = true;

        colorAttachmentCount = rtLayout.numColorOutputs;
        attachmentDescCount = rtLayout.numColorOutputs;
        hasDepthStencilAttachment = false;

        if (rtLayout.depthStencilFormat != RHI::DepthStencilFormat::None) // Depth Stencil Attachment
        {
            hasDepthStencilAttachment = true;

            Array<VkFormat> preferredDepthFormats{};
            if (rtLayout.depthStencilFormat == RHI::DepthStencilFormat::Auto || rtLayout.depthStencilFormat == RHI::DepthStencilFormat::D32_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHI::DepthStencilFormat::D24_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == RHI::DepthStencilFormat::D32)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT });

            auto depthFormat = device->FindSupportedFormat(preferredDepthFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            if (depthFormat == VK_FORMAT_UNDEFINED)
            {
                hasDepthStencilAttachment = false;
            }
            else
            {
                // Valid format found
                auto& depthAttachment = attachmentDesc[attachmentDescCount];
                depthAttachment.flags = 0;

                switch (rtLayout.depthStencilFormat)
                {
                case DepthStencilFormat::Auto:
                    depthAttachment.format = depthFormat;
                    break;
                case DepthStencilFormat::D32_S8:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
                    break;
                case DepthStencilFormat::D24_S8:
                    depthAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
                    break;
                case DepthStencilFormat::D32:
                    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
                    break;
                }
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // TODO: Add stencil later
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                this->depthFormat = depthAttachment.format;

                depthStencilReference.attachment = attachmentDescCount;
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                attachmentDescCount++;
            }
        }

        for (int i = 0; i < rtLayout.numColorOutputs; i++) // Color Attachments
        {
            auto& attachment = attachmentDesc[i];
            attachment = {};
            attachment.flags = 0;

            switch (rtLayout.colorOutputs[i].preferredFormat)
            {
            case ColorFormat::Auto:
                attachment.format = device->FindAutoColorFormat().format;
                break;
            case ColorFormat::RGBA32:
				attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
                break;
            case ColorFormat::BGRA32:
				attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
                break;
            }

            colorFormats[i] = attachment.format;

            attachment.samples = (VkSampleCountFlagBits)rtLayout.colorOutputs[i].sampleCount;

            if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::Clear)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            else if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::Load)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            else if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::None)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

            if (rtLayout.colorOutputs[i].storeAction == RenderPassStoreAction::None)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            else if (rtLayout.colorOutputs[i].storeAction == RenderPassStoreAction::Store)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (i == rtLayout.presentationRTIndex)
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            else
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            auto& colorReference = colorReferences[i];
            colorReference.attachment = i;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
    }

    VulkanRenderTargetLayout::VulkanRenderTargetLayout(VulkanDevice* device, VulkanViewport* viewport, const RHI::RenderTargetLayout& rtLayout)
    {
        using namespace CE::RHI;

        this->width = viewport->GetWidth();
        this->height = viewport->GetHeight();
        this->backBufferCount = viewport->GetBackBufferCount();
        this->simultaneousFrameDraws = viewport->GetBackBufferCount();
        this->presentationRTIndex = rtLayout.presentationRTIndex;
        isValid = true;

        colorAttachmentCount = rtLayout.numColorOutputs;
        attachmentDescCount = rtLayout.numColorOutputs;
        hasDepthStencilAttachment = false;

        if (rtLayout.depthStencilFormat != DepthStencilFormat::None) // Depth Stencil Attachment
        {
            hasDepthStencilAttachment = true;

            Array<VkFormat> preferredDepthFormats{};
            if (rtLayout.depthStencilFormat == DepthStencilFormat::Auto || rtLayout.depthStencilFormat == DepthStencilFormat::D32_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == DepthStencilFormat::D24_S8)
                preferredDepthFormats.AddRange({ VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT });
            else if (rtLayout.depthStencilFormat == DepthStencilFormat::D32)
                preferredDepthFormats.AddRange({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT });

            auto depthFormat = viewport->swapChain->swapChainDepthFormat;

            if (depthFormat == VK_FORMAT_UNDEFINED)
            {
                hasDepthStencilAttachment = false;
            }
            else
            {
                // Valid format found
                auto& depthAttachment = attachmentDesc[attachmentDescCount];
                depthAttachment.flags = 0;
                
                depthAttachment.format = depthFormat;
                    
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // TODO: Add stencil later
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                this->depthFormat = depthAttachment.format;

                depthStencilReference.attachment = attachmentDescCount;
                depthStencilReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                attachmentDescCount++;
            }
        }

        
        for (int i = 0; i < rtLayout.numColorOutputs; i++) // Color Attachments
        {
            auto& attachment = attachmentDesc[i];
            attachment = {};
            attachment.flags = 0;

            if (i == rtLayout.presentationRTIndex)
            {
                attachment.format = viewport->GetSwapChain()->swapChainColorFormat.format;
            }
            else
            {
                switch (rtLayout.colorOutputs[i].preferredFormat)
                {
                case ColorFormat::Auto:
                    attachment.format = device->FindAutoColorFormat().format;
                    break;
                case ColorFormat::RGBA32:
                    if (device->CheckSurfaceFormatSupport(VK_FORMAT_R8G8B8A8_UNORM))
                        attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
                    else
                        attachment.format = device->FindAutoColorFormat().format;
                    break;
                case ColorFormat::BGRA32:
                    if (device->CheckSurfaceFormatSupport(VK_FORMAT_B8G8R8A8_UNORM))
                        attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
                    else
                        attachment.format = device->FindAutoColorFormat().format;
                    break;
                }
            }

            colorFormats[i] = attachment.format;

            attachment.samples = (VkSampleCountFlagBits)rtLayout.colorOutputs[i].sampleCount;

            if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::Clear)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            else if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::Load)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            else if (rtLayout.colorOutputs[i].loadAction == RenderPassLoadAction::None)
                attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

            if (rtLayout.colorOutputs[i].storeAction == RenderPassStoreAction::None)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            else if (rtLayout.colorOutputs[i].storeAction == RenderPassStoreAction::Store)
                attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (i == rtLayout.presentationRTIndex)
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
            else
            {
                attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }

            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            auto& colorReference = colorReferences[i];
            colorReference.attachment = i;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
    }

    // ****************************************************
    // VulkanRenderTarget

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRenderTarget(false)
    {
        this->device = device;
        this->rtLayout = rtLayout;
        this->backBufferCount = rtLayout.backBufferCount;
        this->simultaneousFrameDraws = rtLayout.simultaneousFrameDraws;
        this->width = rtLayout.width;
        this->height = rtLayout.height;
		this->viewport = nullptr;

        // Create render pass
        renderPass = new VulkanRenderPass(device, rtLayout);

        // Depth Buffer
        if (rtLayout.HasDepthStencilAttachment())
        {
            CreateDepthBuffer();
        }

        // Color Buffers
        CreateColorBuffers();
    }

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRenderTarget(true)
    {
        this->device = device;
        this->rtLayout = rtLayout;
        this->viewport = viewport;

        // Create render pass
        renderPass = new VulkanRenderPass(device, rtLayout);

        // No need to create any depth & color buffers, they are handled by VulkanViewport
    }

    VulkanRenderTarget::~VulkanRenderTarget()
    {
        DestroyColorBuffers();
        DestroyDepthBuffer();

        delete renderPass;
    }

    RHI::RenderPass* VulkanRenderTarget::GetRenderPass()
    {
        return renderPass;
    }

    void VulkanRenderTarget::SetClearColor(u32 colorTargetIndex, const Color& color)
    {
        this->clearColors[colorTargetIndex] = color;
    }

	void VulkanRenderTarget::Resize(u32 newWidth, u32 newHeight)
	{
		DestroyDepthBuffer();
		DestroyColorBuffers();

		this->width = newWidth;
		this->height = newHeight;
		this->rtLayout.width = newWidth;
		this->rtLayout.height = newHeight;
		
		// Depth Buffer
		if (rtLayout.HasDepthStencilAttachment())
		{
			CreateDepthBuffer();
		}

		// Color Buffers
		CreateColorBuffers();
	}

	RHI::Texture* VulkanRenderTarget::GetColorTargetTexture(int index, int attachmentIndex)
	{
		if (index < 0 || index >= colorFrames.GetSize())
			return nullptr;
		if (attachmentIndex < 0 || attachmentIndex >= colorFrames[index].textures.GetSize())
			return nullptr;
		return colorFrames[index].textures[attachmentIndex];
	}

	RHI::Sampler* VulkanRenderTarget::GetColorTargetTextureSampler(int index, int attachmentIndex)
	{
		if (index >= colorFrames.GetSize() || index < 0)
			return nullptr;
		if (attachmentIndex < 0 || attachmentIndex >= colorFrames[index].samplers.GetSize())
			return nullptr;
		return colorFrames[index].samplers[attachmentIndex];
	}

    VkRenderPass VulkanRenderTarget::GetVulkanRenderPassHandle() const
    {
        if (renderPass == nullptr)
            return nullptr;
        return renderPass->GetHandle();
    }

    u32 VulkanRenderTarget::GetBackBufferCount()
    {
        if (IsViewportRenderTarget())
            return viewport->GetBackBufferCount();
        return backBufferCount;
    }

    u32 VulkanRenderTarget::GetSimultaneousFrameDrawCount()
    {
        if (IsViewportRenderTarget())
            return viewport->GetSimultaneousFrameDrawCount();
        return simultaneousFrameDraws;
    }

    u32 VulkanRenderTarget::GetWidth()
    {
        if (IsViewportRenderTarget())
            return viewport->GetWidth();
        return width;
    }

    u32 VulkanRenderTarget::GetHeight()
    {
        if (IsViewportRenderTarget())
            return viewport->GetHeight();
        return height;
    }

    void VulkanRenderTarget::CreateDepthBuffer()
    {
        auto imageFormat = rtLayout.depthFormat;

        RHI::TextureDesc textureDesc{};
        textureDesc.name = "Depth Buffer";
        textureDesc.width = width;
        textureDesc.height = height;
        textureDesc.depth = 1;
        textureDesc.dimension = RHI::TextureDimension::Dim2D;
        textureDesc.format = VkFormatToRHITextureFormat(rtLayout.depthFormat);
        textureDesc.mipLevels = 1;
        textureDesc.sampleCount = 1;
        textureDesc.usageFlags = RHI::TextureUsageFlags::DepthStencilAttachment;
        textureDesc.forceLinearLayout = false;

        depthFrame.textures.Resize(1);
        depthFrame.textures[0] = new VulkanTexture(device, textureDesc);

		RHI::SamplerDesc samplerDesc{};
		samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerDesc.enableAnisotropy = true;
		samplerDesc.maxAnisotropy = 16;
		samplerDesc.borderColor = Color::Black();
		samplerDesc.samplerFilterMode = RHI::FILTER_MODE_LINEAR;

		depthFrame.samplers.Resize(1);
		depthFrame.samplers[0] = new VulkanSampler(device, samplerDesc);

		// Old sampler code
        //VkSamplerCreateInfo samplerCI = {};
        //samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        //samplerCI.addressModeU = samplerCI.addressModeV = samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        //samplerCI.anisotropyEnable = VK_FALSE;
        //samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        //samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        //samplerCI.minFilter = samplerCI.magFilter = VK_FILTER_LINEAR;
        //samplerCI.compareEnable = VK_FALSE;
        //samplerCI.minLod = 0.0f;
        //samplerCI.maxLod = 0.0f;
        //samplerCI.mipLodBias = 0.0f;
        //samplerCI.unnormalizedCoordinates = VK_FALSE;

        //depthFrame.samplers.Resize(1);
        //vkCreateSampler(device->GetHandle(), &samplerCI, nullptr, &depthFrame.samplers[0]);

        depthFrame.framebuffer = nullptr;
    }

    void VulkanRenderTarget::DestroyDepthBuffer()
    {
        if (depthFrame.textures.GetSize() > 0)
            delete depthFrame.textures[0];
		if (depthFrame.samplers.GetSize() > 0)
			delete depthFrame.samplers[0];
            //vkDestroySampler(device->GetHandle(), depthFrame.samplers[0], nullptr);
        delete depthFrame.framebuffer;
    }

    void VulkanRenderTarget::CreateColorBuffers()
    {
        for (int i = 0; i < backBufferCount; i++)
        {
            VulkanFrame frame{};
            frame.textures.Resize(rtLayout.colorAttachmentCount);
            frame.samplers.Resize(rtLayout.colorAttachmentCount);

            VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1] = {};

            for (int j = 0; j < rtLayout.colorAttachmentCount; j++)
            {
                auto imageFormat = rtLayout.colorFormats[j];

                RHI::TextureDesc textureDesc{};
                textureDesc.name = "Color Buffer";
                textureDesc.width = GetWidth();
                textureDesc.height = GetHeight();
                textureDesc.depth = 1;
                textureDesc.dimension = RHI::TextureDimension::Dim2D;
                textureDesc.format = VkFormatToRHITextureFormat(imageFormat);
                textureDesc.mipLevels = 1;
                textureDesc.usageFlags = RHI::TextureUsageFlags::ColorAttachment | RHI::TextureUsageFlags::SampledImage;
                textureDesc.sampleCount = 1;
                textureDesc.forceLinearLayout = false;

                frame.textures[j] = new VulkanTexture(device, textureDesc);

				RHI::SamplerDesc samplerDesc{};
				samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = RHI::SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				samplerDesc.enableAnisotropy = true;
				samplerDesc.maxAnisotropy = 16;
				samplerDesc.borderColor = Color::Black();
				samplerDesc.samplerFilterMode = RHI::FILTER_MODE_LINEAR;

				frame.samplers[j] = new VulkanSampler(device, samplerDesc);

                //VkSamplerCreateInfo samplerCI{};
                //samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                //samplerCI.addressModeU = samplerCI.addressModeV = samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                //samplerCI.anisotropyEnable = VK_FALSE;
                //samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
                //samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                //samplerCI.minFilter = samplerCI.magFilter = VK_FILTER_LINEAR;
                //samplerCI.compareEnable = VK_FALSE;
                //samplerCI.minLod = 0.0f;
                //samplerCI.maxLod = 0.0f;
                //samplerCI.mipLodBias = 0.0f;
                //samplerCI.unnormalizedCoordinates = VK_FALSE;

                //vkCreateSampler(device->GetHandle(), &samplerCI, nullptr, &frame.samplers[j]);

                attachments[j] = frame.textures[j]->GetImageView();
            }

            if (rtLayout.HasDepthStencilAttachment())
            {
                attachments[rtLayout.colorAttachmentCount] = depthFrame.textures[0]->GetImageView();
            }

            frame.framebuffer = new VulkanFrameBuffer(device, attachments, this);

            colorFrames.Add(frame);
        }
    }

    void VulkanRenderTarget::DestroyColorBuffers()
    {
        for (int i = 0; i < colorFrames.GetSize(); i++)
        {
            for (auto texture : colorFrames[i].textures)
            {
                delete texture;
            }
            colorFrames[i].textures.Clear();

            for (auto sampler : colorFrames[i].samplers)
            {
				delete sampler;
                //vkDestroySampler(device->GetHandle(), sampler, nullptr);
            }

            delete colorFrames[i].framebuffer;
        }
        colorFrames.Clear();
    }

} // namespace CE

