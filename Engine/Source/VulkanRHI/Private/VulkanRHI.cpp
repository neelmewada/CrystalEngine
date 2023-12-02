
#include "CoreMinimal.h"

#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"
#include "PAL/Common/VulkanPlatform.h"
#undef max

#include "VulkanBuffer.h"
#include "VulkanViewport.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "VulkanShaderModule.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorPool.h"

#include <vulkan/vulkan.h>

CE_IMPLEMENT_MODULE(VulkanRHI, CE::VulkanRHIModule)

namespace CE
{
	void VulkanRHIModule::StartupModule()
	{
        //RHI::gDynamicRHI = new VulkanRHI();
	}

	void VulkanRHIModule::ShutdownModule()
	{
        delete RHI::gDynamicRHI;
	}

	void VulkanRHIModule::RegisterTypes()
	{

	}

    VKAPI_ATTR VkBool32 VulkanValidationCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            CE_LOG(Error, All, "Vulkan Error: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            CE_LOG(Info, All, "Vulkan Info: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            CE_LOG(Info, All, "Vulkan Verbose: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            CE_LOG(Warn, All, "Vulkan Warning: {}", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else 
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        if (VulkanPlatform::IsVerboseValidationEnabled())
            createInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VulkanValidationCallback;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) 
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

	void VulkanRHI::Initialize()
	{
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_1;
        appInfo.pEngineName = CE_ENGINE_NAME_STRING;
        appInfo.engineVersion = VK_MAKE_VERSION(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
        appInfo.pApplicationName = gProjectName.GetCString();
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        
        Array<const char*> validationLayers = VulkanPlatform::GetValidationLayers();
        bool enableValidation = VulkanPlatform::IsValidationEnabled();
        Array<const char*> instanceExtensions = VulkanPlatform::GetRequiredInstanceExtensions();

        VkInstanceCreateInfo instanceCI{};
        instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCI.pApplicationInfo = &appInfo;
        instanceCI.flags = VulkanPlatform::GetVulkanInstanceFlags();

        VkDebugUtilsMessengerCreateInfoEXT debugCI{};
        debugCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCI.pNext = nullptr;

        instanceCI.enabledExtensionCount = instanceExtensions.GetSize();
        instanceCI.ppEnabledExtensionNames = instanceExtensions.GetData();

        if (enableValidation)
        {
            instanceCI.enabledLayerCount = static_cast<uint32_t>(validationLayers.GetSize());
            instanceCI.ppEnabledLayerNames = validationLayers.GetData();

            PopulateDebugMessengerCreateInfo(debugCI);
            instanceCI.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCI;
        }
        else
        {
            instanceCI.enabledLayerCount = 0;
            instanceCI.ppEnabledLayerNames = nullptr;
            instanceCI.pNext = nullptr;
        }
        
        u32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        Array<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());
        
        VkResult result = vkCreateInstance(&instanceCI, nullptr, &vkInstance);
        if (result != VK_SUCCESS)
        {
            CE_LOG(Critical, All, "Failed to create vulkan instance. Result = {}" , (int)result);
            return;
        }

        if (enableValidation && CreateDebugUtilsMessengerEXT(vkInstance, &debugCI, nullptr, &vkMessenger) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan debug messenger!");
            return;
        }
	}

    void VulkanRHI::PostInitialize()
    {
        device = new VulkanDevice(vkInstance, this);
        device->Initialize();
    }

	void VulkanRHI::PreShutdown()
	{
        if (device != nullptr)
        {
            device->PreShutdown();
        }
	}

	void VulkanRHI::Shutdown()
	{
        if (device != nullptr)
        {
            device->Shutdown();
            delete device;
            device = nullptr;
        }

        if (VulkanPlatform::IsValidationEnabled())
        {
            DestroyDebugUtilsMessengerEXT(vkInstance, vkMessenger, nullptr);
        }

        vkDestroyInstance(vkInstance, nullptr);
        vkInstance = nullptr;
	}

    void* VulkanRHI::GetNativeHandle()
    {
        return vkInstance;
    }

    RHI::GraphicsBackend VulkanRHI::GetGraphicsBackend()
	{
		return RHI::GraphicsBackend::Vulkan;
	}

    // - Render Target -

	Vec2i VulkanRHI::GetScreenSizeForWindow(void* platformWindowHandle)
	{
		return VulkanPlatform::GetScreenSizeForWindow(platformWindowHandle);
	}

	RHI::RenderTarget* VulkanRHI::CreateRenderTarget(u32 width, u32 height,
        const RHI::RenderTargetLayout& rtLayout)
    {
        return new VulkanRenderTarget(device, VulkanRenderTargetLayout(device, width, height, rtLayout));
    }

    void VulkanRHI::DestroyRenderTarget(RHI::RenderTarget* renderTarget)
    {
        delete renderTarget;
    } 

    RHI::Viewport* VulkanRHI::CreateViewport(PlatformWindow* window, u32 width, u32 height, bool isFullscreen, const RHI::RenderTargetLayout& rtLayout)
    {
        return new VulkanViewport(this, device, window, width, height, isFullscreen, rtLayout);
    }

    void VulkanRHI::DestroyViewport(RHI::Viewport* viewport)
    {
        delete viewport;
    }

    // - Command List -

    RHI::GraphicsCommandList* VulkanRHI::CreateGraphicsCommandList(RHI::Viewport* viewport)
    {
        return new VulkanGraphicsCommandList(this, device, (VulkanViewport*)viewport);
    }

    RHI::GraphicsCommandList* VulkanRHI::CreateGraphicsCommandList(RHI::RenderTarget* renderTarget)
    {
        if (renderTarget->IsViewportRenderTarget())
        {
            auto vulkanViewport = ((VulkanRenderTarget*)renderTarget)->GetVulkanViewport();
            return CreateGraphicsCommandList(vulkanViewport);
        }

        return new VulkanGraphicsCommandList(this, device, (VulkanRenderTarget*)renderTarget);
    }

    void VulkanRHI::DestroyCommandList(RHI::CommandList* commandList)
    {
        delete commandList;
    }

    bool VulkanRHI::ExecuteCommandList(RHI::CommandList* commandList)
    {
        if (commandList->GetCommandListType() == RHI::CommandListType::Graphics)
        {
            auto vulkanCommandList = (VulkanGraphicsCommandList*)commandList;
            if (vulkanCommandList->IsViewportTarget())
            {
                return ExecuteGraphicsCommandList(vulkanCommandList, vulkanCommandList->GetViewport());
            }
            else
            {
                return ExecuteGraphicsCommandList(vulkanCommandList, vulkanCommandList->GetRenderTarget());
            }
        }
        return false;
    }

    bool VulkanRHI::ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanRenderTarget* renderTarget)
    {
        constexpr auto u64Max = std::numeric_limits<u64>::max();

        // -- Waiting for Fences --
        // Wait until the rendering from previous call has been finished
        vkWaitForFences(device->GetHandle(),
            1, &commandList->renderFinishedFence[renderTarget->currentImageIndex],
            VK_TRUE, u64Max);

        // Manually reset (close) the fences
        vkResetFences(device->GetHandle(), 1, &commandList->renderFinishedFence[renderTarget->currentImageIndex]);

        // Submit to Queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // Wait until rendering is finished on this frame
        submitInfo.waitSemaphoreCount = renderTarget->isFresh ? 0 : 1;
        submitInfo.pWaitSemaphores = &commandList->renderFinishedSemaphore[renderTarget->currentImageIndex];

        // Execute the whole command buffer until the fragment shader stage (so we don't output any data to framebuffer yet)
        // and THEN wait for the image to be available (m_ImageAvailableForRendering) so fragment pShader can output pixels to it.
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.pWaitDstStageMask = waitStages;

        // Only submit the command buffer we want to render to since they're 1:1 with SwapChain Images
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandList->commandBuffers[renderTarget->currentImageIndex];

        // Semaphores to signal when command buffer finishes executing (finished rendering)
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &commandList->renderFinishedSemaphore[renderTarget->currentImageIndex]; // 1:1 with frames (0..<=MaxSimultaneousFrameDraws)

        vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo,
            commandList->renderFinishedFence[renderTarget->currentImageIndex]);

		// TODO: For testing purposes only
		//vkQueueWaitIdle(device->GetGraphicsQueue()->GetHandle());

        renderTarget->isFresh = false;
        return true;
    }

    bool VulkanRHI::ExecuteGraphicsCommandList(VulkanGraphicsCommandList* commandList, VulkanViewport* viewport)
    {
        constexpr auto u64Max = std::numeric_limits<u64>::max();

        // -- Waiting for Fences --
        // Wait until the rendering from previous call has been finished
        auto result = vkWaitForFences(device->GetHandle(),
            1, &commandList->renderFinishedFence[viewport->currentImageIndex],
            VK_TRUE, u64Max);
        
        result = vkAcquireNextImageKHR(device->GetHandle(), viewport->swapChain->GetHandle(), u64Max,
            viewport->imageAcquiredSemaphore[viewport->currentDrawFrameIndex], VK_NULL_HANDLE, &viewport->currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            viewport->Rebuild();
            return false;
        }
        
        // Manually reset (close) the fences
        result = vkResetFences(device->GetHandle(), 1, &commandList->renderFinishedFence[viewport->currentImageIndex]);

        // Submit to Queue
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        // List of semaphores to wait on at waitStages[i]
        submitInfo.pWaitSemaphores = &viewport->imageAcquiredSemaphore[viewport->currentDrawFrameIndex];

        // Execute the whole command buffer until the fragment shader stage (so we don't output any data to framebuffer yet)
        // and THEN wait for the image to be available (imageAcquiredSemaphore) so fragment shader can output pixels to it.
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.pWaitDstStageMask = waitStages; // Stages to check semaphores at

        // Only submit the command buffer we want to render to since they're 1:1 with SwapChain Images
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandList->commandBuffers[viewport->currentImageIndex];

        // Semaphores to signal when command buffer finishes executing (finished rendering)
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &commandList->renderFinishedSemaphore[viewport->currentImageIndex]; // 1:1 with frames (NumCommandBuffers)

        result = vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo, commandList->renderFinishedFence[viewport->currentImageIndex]);

        return true;
    }

    bool VulkanRHI::PresentViewport(RHI::GraphicsCommandList* viewportCommandList)
    {
        auto vulkanCommandList = (VulkanGraphicsCommandList*)viewportCommandList;
        if (!vulkanCommandList->IsViewportTarget() || vulkanCommandList->viewport == nullptr)
            return false;

        auto vulkanViewport = vulkanCommandList->GetViewport();
        auto swapChain = vulkanViewport->GetSwapChain()-> GetHandle();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &vulkanCommandList->renderFinishedSemaphore[vulkanViewport->currentImageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = &vulkanViewport->currentImageIndex;

        auto result = vkQueuePresentKHR(device->GetGraphicsQueue()->GetHandle(), &presentInfo);
        if (result != VK_SUCCESS)
            return false;

        // Next frame index (if we're rendering 2 frames simultaneously for triple buffering)
        vulkanViewport->currentDrawFrameIndex = (vulkanViewport->currentDrawFrameIndex + 1) % vulkanViewport->GetSimultaneousFrameDrawCount();

        return true;
    }

    // - Resources -

    RHI::Buffer* VulkanRHI::CreateBuffer(const RHI::BufferDesc& bufferDesc)
    {
        return new VulkanBuffer(device, bufferDesc);
    }

    void VulkanRHI::DestroyBuffer(RHI::Buffer* buffer)
    {
        delete buffer;
    }

    RHI::Texture* VulkanRHI::CreateTexture(const RHI::TextureDesc& textureDesc)
    {
        return new VulkanTexture(device, textureDesc);
    }

    void VulkanRHI::DestroyTexture(RHI::Texture* texture)
    {
        delete texture;
    }

    RHI::Sampler* VulkanRHI::CreateSampler(const RHI::SamplerDesc& samplerDesc)
    {
        return new VulkanSampler(device, samplerDesc);
    }

    void VulkanRHI::DestroySampler(RHI::Sampler* sampler)
    {
        delete sampler;
    }

    void* VulkanRHI::AddImGuiTexture(RHI::Texture* texture, RHI::Sampler* sampler)
    {
        return VulkanPlatform::AddImGuiTexture((VulkanTexture*)texture, (VulkanSampler*)sampler);
    }

    void VulkanRHI::RemoveImGuiTexture(void* imguiTexture)
    {
		device->WaitUntilIdle();
        VulkanPlatform::RemoveImGuiTexture((VkDescriptorSet)imguiTexture);
    }

	RHI::ShaderModule* VulkanRHI::CreateShaderModule(const RHI::ShaderModuleDesc& desc)
	{
		return new VulkanShaderModule(device, desc);
	}

	void VulkanRHI::DestroyShaderModule(RHI::ShaderModule* shaderModule)
	{
		delete shaderModule;
	}

	RHI::ShaderResourceGroup* VulkanRHI::CreateShaderResourceGroup(const RHI::ShaderResourceGroupDesc& desc)
	{
		return new VulkanShaderResourceGroup(device, desc);
	}

	void VulkanRHI::DestroyShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup)
	{
		delete shaderResourceGroup;
	}

	RHI::GraphicsPipelineState* VulkanRHI::CreateGraphicsPipelineState(RHI::RenderTarget* renderTarget, const RHI::GraphicsPipelineDesc& desc)
	{
		return new VulkanGraphicsPipeline(device, (VulkanRenderTarget*)renderTarget, desc);
	}

	void VulkanRHI::DestroyPipelineState(RHI::IPipelineState* pipelineState)
	{
		delete pipelineState;
	}

    /******************************************************************************
    *  VulkanFrameBuffer
    */

    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, u32 swapChainImageIndex, VulkanRenderTarget* renderTarget)
    {
        this->device = device;
        const auto& rtLayout = renderTarget->rtLayout;

        VkFramebufferCreateInfo frameBufferCI{};
        frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        
        frameBufferCI.attachmentCount = rtLayout.colorAttachmentCount + (rtLayout.HasDepthStencilAttachment() ? 1 : 0);
        
        List<VkImageView> attachments{ frameBufferCI.attachmentCount };
        attachments[rtLayout.presentationRTIndex] = swapChain->swapChainColorImages[swapChainImageIndex].imageView;

        if (rtLayout.HasDepthStencilAttachment())
        {
            attachments[attachments.GetSize() - 1] = swapChain->swapChainDepthImage->GetImageView();
        }

        frameBufferCI.pAttachments = attachments.GetData();

        frameBufferCI.width = swapChain->GetWidth();
        frameBufferCI.height = swapChain->GetHeight();

        frameBufferCI.renderPass = renderTarget->GetVulkanRenderPass()->GetHandle();

        frameBufferCI.layers = 1;

        if (vkCreateFramebuffer(device->GetHandle(), &frameBufferCI, nullptr, &frameBuffer) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan Frame Buffer");
            return;
        }
    }

    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, 
        VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], 
        VulkanRenderTarget* renderTarget)
    {
        this->device = device;
        const auto& rtLayout = renderTarget->rtLayout;

        VkFramebufferCreateInfo frameBufferCI{};
        frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        frameBufferCI.attachmentCount = rtLayout.colorAttachmentCount + (rtLayout.HasDepthStencilAttachment() ? 1 : 0);
        frameBufferCI.pAttachments = attachments;

        frameBufferCI.width = renderTarget->GetWidth();
        frameBufferCI.height = renderTarget->GetHeight();

        frameBufferCI.renderPass = renderTarget->GetVulkanRenderPass()->GetHandle();

        frameBufferCI.layers = 1;

        if (vkCreateFramebuffer(device->GetHandle(), &frameBufferCI, nullptr, &frameBuffer) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to create Vulkan Frame Buffer");
            return;
        }
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);
    }

    /******************************************************************************
     *  VulkanGraphicsCommandList
     */

    VulkanGraphicsCommandList::VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport)
        : vulkanRHI(vulkanRHI)
        , device(device)
        , viewport(viewport)
    {
        this->renderTarget = (VulkanRenderTarget*)viewport->GetRenderTarget();
        this->numCommandBuffers = viewport->GetBackBufferCount();
        this->simultaneousFrameDraws = viewport->GetSimultaneousFrameDrawCount();

        VkCommandBufferAllocateInfo commandAllocInfo = {};
        commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandAllocInfo.commandBufferCount = numCommandBuffers;
        commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

        commandBuffers.Resize(numCommandBuffers);

        if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
        {
            CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
            return;
        }

        CreateSyncObjects();
    }

    VulkanGraphicsCommandList::VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget)
        : vulkanRHI(vulkanRHI)
        , device(device)
        , renderTarget(renderTarget)
    {
        if (renderTarget->IsViewportRenderTarget())
        {
            auto viewport = renderTarget->GetVulkanViewport();

            this->renderTarget = (VulkanRenderTarget*)viewport->GetRenderTarget();
            this->numCommandBuffers = viewport->GetBackBufferCount();
            this->simultaneousFrameDraws = viewport->GetSimultaneousFrameDrawCount();

            VkCommandBufferAllocateInfo commandAllocInfo = {};
            commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandAllocInfo.commandBufferCount = numCommandBuffers;
            commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

            commandBuffers.Resize(numCommandBuffers);

            if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
                return;
            }

            CreateSyncObjects();
            return;
        }
        else
        {
            this->numCommandBuffers = renderTarget->GetBackBufferCount();
            this->simultaneousFrameDraws = renderTarget->GetBackBufferCount();

            VkCommandBufferAllocateInfo commandAllocInfo = {};
            commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandAllocInfo.commandBufferCount = numCommandBuffers;
            commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandAllocInfo.commandPool = device->GetGraphicsCommandPool();

            commandBuffers.Resize(numCommandBuffers);

            if (vkAllocateCommandBuffers(device->GetHandle(), &commandAllocInfo, commandBuffers.GetData()) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to allocate Vulkan Command Buffers for a Graphics Command List object!");
                return;
            }

            CreateSyncObjects();
        }
    }

    VulkanGraphicsCommandList::~VulkanGraphicsCommandList()
    {
        vkDeviceWaitIdle(device->GetHandle());

        vkFreeCommandBuffers(device->GetHandle(), device->GetGraphicsCommandPool(), numCommandBuffers, commandBuffers.GetData());

        DestroySyncObjects();
    }

    void VulkanGraphicsCommandList::Begin()
    {
        constexpr auto u64Max = std::numeric_limits<u64>::max();

        vkWaitForFences(device->GetHandle(), renderFinishedFence.GetSize(), renderFinishedFence.GetData(), VK_TRUE, u64Max);

        VkExtent2D extent{};
        extent.width = renderTarget->GetWidth();
        extent.height = renderTarget->GetHeight();

        // - Dynamic Viewport & Scissor -
        VkViewport viewportSize = {};
        viewportSize.x = 0;
        viewportSize.y = 0;
        viewportSize.width = (float)extent.width;
        viewportSize.height = (float)extent.height;
        viewportSize.minDepth = 0.0f;
        viewportSize.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        // - Command Buffer & Render Pass --
        VkCommandBufferBeginInfo cmdBufferInfo{};
        cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = ((VulkanRenderPass*)renderTarget->GetRenderPass())->GetHandle();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        
        VkClearValue clearValues[RHI::MaxSimultaneousRenderOutputs + 1] = {};
        for (int i = 0; i < renderTarget->GetColorAttachmentCount(); ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                clearValues[i].color.float32[j] = renderTarget->clearColors[i][j];
            }
        }

        if (renderTarget->HasDepthStencilAttachment())
        {
            clearValues[renderTarget->GetColorAttachmentCount()].depthStencil.depth = 1.0f;
            clearValues[renderTarget->GetColorAttachmentCount()].depthStencil.stencil = 0;
        }

        renderPassInfo.clearValueCount = renderTarget->GetTotalAttachmentCount();
        renderPassInfo.pClearValues = clearValues;

        for (int i = 0; i < commandBuffers.GetSize(); i++)
        {
            if (viewport != nullptr)
            {
                renderPassInfo.framebuffer = viewport->frameBuffers[i]->GetHandle();
            }
            else
            {
                renderPassInfo.framebuffer = renderTarget->colorFrames[i].framebuffer->GetHandle();
            }

            // Begin Command Buffer
            vkBeginCommandBuffer(commandBuffers[i], &cmdBufferInfo);

            // Begin Render Pass
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Set viewport & scissor dynamic state
            vkCmdSetViewport(commandBuffers[i], 0, 1, &viewportSize);
            vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);
        }
    }

    void VulkanGraphicsCommandList::End()
    {
        for (int i = 0; i < commandBuffers.GetSize(); ++i)
        {
            vkCmdEndRenderPass(commandBuffers[i]);
            vkEndCommandBuffer(commandBuffers[i]);
        }
    }

	void VulkanGraphicsCommandList::BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers)
	{
		List<VkBuffer> vkBuffers = {};
		Array<SIZE_T> offsets = {};
		vkBuffers.Resize(buffers.GetSize());
		offsets.Resize(buffers.GetSize());
		for (int i = 0; i < buffers.GetSize(); i++)
		{
			vkBuffers[i] = (VkBuffer)buffers[i]->GetHandle();
			offsets[i] = 0;
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindVertexBuffers(commandBuffers[i], firstBinding, vkBuffers.GetSize(), vkBuffers.GetData(), offsets.GetData());
		}
	}

	void VulkanGraphicsCommandList::BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets)
	{
		if (buffers.GetSize() != bufferOffsets.GetSize())
		{
			CE_LOG(Error, All, "BindVertexBuffers() passed with buffers & bufferOffsets array of different size!");
			return;
		}

		List<VkBuffer> vkBuffers = {};
		vkBuffers.Resize(buffers.GetSize());
		for (int i = 0; i < buffers.GetSize(); i++)
		{
			vkBuffers[i] = (VkBuffer)buffers[i]->GetHandle();
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindVertexBuffers(commandBuffers[i], firstBinding, vkBuffers.GetSize(), vkBuffers.GetData(), bufferOffsets.GetData());
		}
	}

	void VulkanGraphicsCommandList::BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset)
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindIndexBuffer(commandBuffers[i], (VkBuffer)buffer->GetHandle(), offset, use32BitIndex ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
		}
	}

	void VulkanGraphicsCommandList::BindPipeline(RHI::IPipelineState* pipeline)
	{
		if (pipeline == nullptr)
			return;

		VkPipeline vkPipeline = (VkPipeline)pipeline->GetNativeHandle();

		RHI::IPipelineLayout* pipelineLayout = pipeline->GetPipelineLayout();

		VkPipelineBindPoint bindPoint{};

		switch (pipelineLayout->GetPipelineType())
		{
		case RHI::PipelineType::None:
			return;
		case RHI::PipelineType::Graphics:
			bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		case RHI::PipelineType::Compute:
			bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			break;
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindPipeline(commandBuffers[i], bindPoint, vkPipeline);
		}
	}

	void VulkanGraphicsCommandList::CommitShaderResources(u32 firstFrequencyId, 
		const List<RHI::ShaderResourceGroup*>& shaderResourceGroups, 
		RHI::IPipelineLayout* pipelineLayout)
	{
		if (shaderResourceGroups.IsEmpty() || pipelineLayout == nullptr)
			return;

		List<VkDescriptorSet> srgs = shaderResourceGroups.Transform<VkDescriptorSet>(
			[&](RHI::ShaderResourceGroup* in) { return ((VulkanShaderResourceGroup*)in)->GetDescriptorSet(); });
		
		VulkanPipelineLayout* vulkanPipelineLayout = (VulkanPipelineLayout*)pipelineLayout;

		VkPipelineBindPoint bindPoint{};

		switch (pipelineLayout->GetPipelineType())
		{
		case RHI::PipelineType::None:
			return;
		case RHI::PipelineType::Graphics:
			bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			break;
		case RHI::PipelineType::Compute:
			bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			break;
		}

		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdBindDescriptorSets(commandBuffers[i], bindPoint, vulkanPipelineLayout->handle, firstFrequencyId, srgs.GetSize(), srgs.GetData(), 0, nullptr);
		}
	}

	void VulkanGraphicsCommandList::WaitForExecution()
	{
		constexpr u64 u64Max = NumericLimits<u64>::Max();
		auto result = vkWaitForFences(device->GetHandle(),
			renderFinishedFence.GetSize(), renderFinishedFence.GetData(),
			VK_TRUE, u64Max);
	}

	void VulkanGraphicsCommandList::DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance)
	{
		for (int i = 0; i < commandBuffers.GetSize(); ++i)
		{
			vkCmdDrawIndexed(commandBuffers[i], indexCount, instanceCount, firstIndex, vertexOffset, firstIndex);
		}
	}

    void VulkanGraphicsCommandList::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreCI{};
        semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        renderFinishedSemaphore.Resize(numCommandBuffers);
        for (int i = 0; i < numCommandBuffers; i++)
        {
            if (vkCreateSemaphore(device->GetHandle(), &semaphoreCI, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create render finished semaphore for a Vulkan Graphics Command List");
                return;
            }
        }

        VkFenceCreateInfo fenceCI = {};
        fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create fence in the signaled state (i.e. fence is open)

        renderFinishedFence.Resize(numCommandBuffers);
        for (int i = 0; i < numCommandBuffers; i++)
        {
            if (vkCreateFence(device->GetHandle(), &fenceCI, nullptr, &renderFinishedFence[i]) != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create render finished fence for a Vulkan Graphics Command List");
                return;
            }
        }
    }

    void VulkanGraphicsCommandList::DestroySyncObjects()
    {
        for (int i = 0; i < renderFinishedFence.GetSize(); ++i)
        {
            vkDestroyFence(device->GetHandle(), renderFinishedFence[i], nullptr);
        }
        renderFinishedFence.Clear();

        for (int i = 0; i < renderFinishedSemaphore.GetSize(); ++i)
        {
            vkDestroySemaphore(device->GetHandle(), renderFinishedSemaphore[i], nullptr);
        }
        renderFinishedSemaphore.Clear();
    }

	/*
	*	Vulkan Shader Resources
	*/

	VulkanShaderResourceGroup::VulkanShaderResourceGroup(VulkanDevice* device, const RHI::ShaderResourceGroupDesc& desc) 
		: device(device), desc(desc)
	{
		VkDescriptorSetLayoutCreateInfo setLayoutCI{};
		setLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		
		bindings.Clear();
		variableNames.Clear();

		for (int i = 0; i < desc.variables.GetSize(); i++)
		{
			const auto& variable = desc.variables[i];

			VkDescriptorSetLayoutBinding binding{};
			binding.binding = variable.binding;
			binding.descriptorCount = variable.arrayCount;
			binding.stageFlags = 0;
			
			if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Vertex))
				binding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			if (EnumHasFlag(variable.stageFlags, RHI::ShaderStage::Fragment))
				binding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

			if (binding.stageFlags == 0)
				continue;

			switch (variable.type)
			{
			case RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER:
				binding.descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			case RHI::SHADER_RESOURCE_TYPE_TEXTURE_BUFFER:
				binding.descriptorType = variable.isDynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case RHI::SHADER_RESOURCE_TYPE_SAMPLED_IMAGE:
				binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				break;
			case RHI::SHADER_RESOURCE_TYPE_SAMPLER_STATE:
				binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				break;
			default:
				continue;
			}
			
			variableNames.Add(variable.name);
			bindings.Add(binding);
		}

		setLayoutCI.bindingCount = bindings.GetSize();
		setLayoutCI.pBindings = bindings.GetData();
		
		auto result = vkCreateDescriptorSetLayout(device->GetHandle(), &setLayoutCI, nullptr, &setLayout);
		if (result != VK_SUCCESS)
		{
			CE_LOG(Error, All, "Failed to create Descriptor Set Layout");
			return;
		}

		auto pool = device->GetDescriptorPool();

		auto sets = pool->Allocate(1, { setLayout }, descriptorPool);
		if (sets.IsEmpty())
		{
			return;
		}

		descriptorSet = sets[0];
	}

	VulkanShaderResourceGroup::~VulkanShaderResourceGroup()
	{
		vkFreeDescriptorSets(device->GetHandle(), descriptorPool, 1, &descriptorSet);
		descriptorPool = nullptr;
		descriptorSet = nullptr;

		vkDestroyDescriptorSetLayout(device->GetHandle(), setLayout, nullptr);
		setLayout = nullptr;
	}

	void VulkanShaderResourceGroup::Bind(Name variableName, RHI::Buffer* buffer)
	{
		int index = variableNames.IndexOf(variableName);
		if (index < 0)
			return;

		bufferVariableBindings[variableName] = buffer;

		VkDescriptorSetLayoutBinding binding = bindings[index];

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = (VkBuffer)buffer->GetHandle();
		bufferInfo.offset = 0;
		bufferInfo.range = buffer->GetBufferSize();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptorSet;
		write.dstBinding = binding.binding;
		write.dstArrayElement = 0;

		write.descriptorCount = binding.descriptorCount;
		write.descriptorType = binding.descriptorType;
		write.pBufferInfo = &bufferInfo;
		
		vkUpdateDescriptorSets(device->GetHandle(), 1, &write, 0, nullptr);
	}

} // namespace CE
