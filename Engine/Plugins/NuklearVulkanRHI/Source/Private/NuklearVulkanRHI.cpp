
#include "CoreMinimal.h"

#include "NuklearVulkanRHI.h"
#include "VulkanRHIPrivate.h"
#include "PAL/Common/VulkanPlatform.h"
#undef max

#include "VulkanBuffer.h"
#include "VulkanViewport.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>

CE_IMPLEMENT_PLUGIN(NuklearVulkanRHI, CE::NuklearVulkanRHIModule)

namespace CE
{
	void NuklearVulkanRHIModule::StartupModule()
	{

	}

	void NuklearVulkanRHIModule::ShutdownModule()
	{

	}

	void NuklearVulkanRHIModule::RegisterTypes()
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

	void NuklearVulkanRHI::Initialize()
	{
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.pEngineName = CE_ENGINE_NAME_STRING;
        appInfo.engineVersion = VK_MAKE_VERSION(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH);
        appInfo.pApplicationName = ProjectSettings::Get().GetProjectName().GetCString();
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        
        CE::Array<const char*> validationLayers = VulkanPlatform::GetValidationLayers();
        bool enableValidation = VulkanPlatform::IsValidationEnabled();
        CE::Array<const char*> instanceExtensions = VulkanPlatform::GetRequiredInstanceExtensions();

        VkInstanceCreateInfo instanceCI{};
        instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCI.pApplicationInfo = &appInfo;

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

    void NuklearVulkanRHI::PostInitialize()
    {
        device = new VulkanDevice(vkInstance, this);
        device->Initialize();
    }

	void NuklearVulkanRHI::PreShutdown()
	{
        if (device != nullptr)
        {
            device->PreShutdown();
        }
	}

	void NuklearVulkanRHI::Shutdown()
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

    void* NuklearVulkanRHI::GetNativeHandle()
    {
        return vkInstance;
    }

	RHIGraphicsBackend NuklearVulkanRHI::GetGraphicsBackend()
	{
		return RHIGraphicsBackend::Vulkan;
	}

    // - Render Target -

    RHIRenderTarget* NuklearVulkanRHI::CreateRenderTarget(u32 width, u32 height, 
        const RHIRenderTargetLayout& rtLayout)
    {
        return new VulkanRenderTarget(device, VulkanRenderTargetLayout(device, width, height, rtLayout));
    }

    void NuklearVulkanRHI::DestroyRenderTarget(RHIRenderTarget* renderTarget)
    {
        delete renderTarget;
    }

    RHIViewport* NuklearVulkanRHI::CreateViewport(void* windowHandle, u32 width, u32 height, bool isFullscreen, const RHIRenderTargetLayout& rtLayout)
    {
        return new VulkanViewport(this, device, windowHandle, width, height, isFullscreen, rtLayout);
    }

    void NuklearVulkanRHI::DestroyViewport(RHIViewport* viewport)
    {
        delete viewport;
    }

    RHIBuffer* NuklearVulkanRHI::CreateBuffer(const RHIBufferDesc& bufferDesc)
    {
        return new VulkanBuffer(device, bufferDesc);
    }

    void NuklearVulkanRHI::DestroyBuffer(RHIBuffer* buffer)
    {
        delete buffer;
    }

    /******************************************************************************
    *  VulkanFrameBuffer
    */

    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderTarget* renderTarget)
    {
        this->device = device;
        const auto& rtLayout = renderTarget->rtLayout;

        VkFramebufferCreateInfo frameBufferCI{};
        frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        
        frameBufferCI.attachmentCount = rtLayout.colorAttachmentCount + (rtLayout.HasDepthStencilAttachment() ? 1 : 0);
        
        Array<VkImageView> attachments{ frameBufferCI.attachmentCount };
        for (int i = 0; i < attachments.GetSize(); i++)
        {
            attachments[i] = swapChain->swapChainColorImages[i].imageView;
        }
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

        CE_LOG(Info, All, "Created Vulkan Frame Buffer");
    }

    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, 
        VkImageView attachments[RHIMaxSimultaneousRenderOutputs + 1], 
        VulkanRenderTarget* renderTarget)
    {
        this->device = device;

    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        vkDestroyFramebuffer(device->GetHandle(), frameBuffer, nullptr);

        CE_LOG(Info, All, "Destroyed Vulkan Frame Buffer");
    }

    /******************************************************************************
     *  VulkanGraphicsCommandList
     */

    VulkanGraphicsCommandList::VulkanGraphicsCommandList(NuklearVulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport)
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
        
        VkClearValue clearValues[RHIMaxSimultaneousRenderOutputs + 1] = {};
        for (int i = 0; i < renderTarget->GetColorAttachmentCount(); ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                clearValues[i].color.float32[j] = renderTarget->clearColors[i][j];
            }
        }

        if (renderTarget->HasDepthStencilAttachment())
            clearValues[renderTarget->GetColorAttachmentCount()].depthStencil.depth = 1.0f;

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
                //renderPassInfo.framebuffer = renderTarget->frames[i].FrameBuffer->GetHandle();
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

} // namespace CE
