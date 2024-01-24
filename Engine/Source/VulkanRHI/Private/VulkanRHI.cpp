
#include "CoreMinimal.h"

#include "VulkanRHI.h"
#include "VulkanRHIPrivate.h"
#include "PAL/Common/VulkanPlatform.h"
#undef max

#include <vulkan/vulkan.h>

CE_IMPLEMENT_MODULE(VulkanRHI, CE::Vulkan::VulkanRHIModule)

namespace CE::Vulkan
{
	static VulkanRHI* gVulkanRHI = nullptr;

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
			if (gVulkanRHI != nullptr)
				gVulkanRHI->BroadCastValidationMessage(ValidationMessageType::Error, pCallbackData->pMessage);
            CE_LOG(Error, All, "Vulkan Error: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
			if (gVulkanRHI != nullptr)
				gVulkanRHI->BroadCastValidationMessage(ValidationMessageType::Info, pCallbackData->pMessage);
            CE_LOG(Info, All, "Vulkan Info: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
			if (gVulkanRHI != nullptr)
				gVulkanRHI->BroadCastValidationMessage(ValidationMessageType::Verbose, pCallbackData->pMessage);
            CE_LOG(Info, All, "Vulkan Verbose: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
			if (gVulkanRHI != nullptr)
				gVulkanRHI->BroadCastValidationMessage(ValidationMessageType::Warning, pCallbackData->pMessage);
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
		gVulkanRHI = this;

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

		gVulkanRHI = nullptr;
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

	RHI::Scope* VulkanRHI::CreateScope(const RHI::ScopeDescriptor& desc)
	{
		return new Vulkan::Scope(device, desc);
	}

	RHI::FrameGraphCompiler* VulkanRHI::CreateFrameGraphCompiler()
	{
		return new Vulkan::FrameGraphCompiler(device);
	}

	RHI::FrameGraphExecuter* VulkanRHI::CreateFrameGraphExecuter()
	{
		return new Vulkan::FrameGraphExecuter(device);
	}

	void VulkanRHI::BroadCastValidationMessage(RHI::ValidationMessageType type, const char* message)
	{
		for (auto handler : validationCallbackHandlers[(int)type])
		{
			if (handler)
				handler(type, message);
		}
	}

	bool VulkanRHI::IsOffscreenOnly()
	{
		return device->IsOffscreenOnly();
	}

	Vec2i VulkanRHI::GetScreenSizeForWindow(void* platformWindowHandle)
	{
		return VulkanPlatform::GetScreenSizeForWindow(platformWindowHandle);
	}

	Array<RHI::Format> VulkanRHI::GetAvailableDepthStencilFormats()
	{
		return device->GetAvailableDepthStencilFormats();
	}

    // - Command List -

    void VulkanRHI::DestroyCommandList(RHI::CommandList* commandList)
    {
        delete commandList;
    }

    bool VulkanRHI::ExecuteCommandList(RHI::CommandList* commandList)
    {
        /*if (commandList->GetCommandListType() == RHI::CommandListType::Graphics)
        {
            auto vulkanCommandList = (GraphicsCommandList*)commandList;
            if (vulkanCommandList->IsViewportTarget())
            {
                return ExecuteGraphicsCommandList(vulkanCommandList, vulkanCommandList->GetViewport());
            }
            else
            {
                return ExecuteGraphicsCommandList(vulkanCommandList, vulkanCommandList->GetRenderTarget());
            }
        }*/
        return false;
    }

    bool VulkanRHI::ExecuteGraphicsCommandList(GraphicsCommandList* commandList, RenderTarget* renderTarget)
    {
        constexpr auto u64Max = std::numeric_limits<u64>::max();

        // -- Waiting for Fences --
        // Wait until the rendering from previous call has been finished
        /*vkWaitForFences(device->GetHandle(),
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
        
        renderTarget->isFresh = false;*/
        return true;
    }
    
    bool VulkanRHI::ExecuteGraphicsCommandList(GraphicsCommandList* commandList, Viewport* viewport)
    {
        constexpr auto u64Max = std::numeric_limits<u64>::max();
		
        // -- Waiting for Fences --
        // Wait until the rendering from previous call has been finished
        //auto result = vkWaitForFences(device->GetHandle(),
        //    1, &commandList->renderFinishedFence[viewport->currentImageIndex],
        //    VK_TRUE, u64Max);
        //
        //result = vkAcquireNextImageKHR(device->GetHandle(), viewport->swapChain->GetHandle(), u64Max,
        //    viewport->imageAcquiredSemaphore[viewport->currentDrawFrameIndex], VK_NULL_HANDLE, &viewport->currentImageIndex);

        //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        //{
        //    viewport->Rebuild();
        //    return false;
        //}
        //
        //// Manually reset (close) the fences
        //result = vkResetFences(device->GetHandle(), 1, &commandList->renderFinishedFence[viewport->currentImageIndex]);
		
        //// Submit to Queue
        //VkSubmitInfo submitInfo = {};
        //submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //submitInfo.waitSemaphoreCount = 1;
        //// List of semaphores to wait on at waitStages[i]
        //submitInfo.pWaitSemaphores = &viewport->imageAcquiredSemaphore[viewport->currentDrawFrameIndex];

        //// Execute the whole command buffer until the fragment shader stage (so we don't output any data to framebuffer yet)
        //// and THEN wait for the image to be available (imageAcquiredSemaphore) so fragment shader can output pixels to it.
        //VkPipelineStageFlags waitStages[] = {
        //        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        //};
        //submitInfo.pWaitDstStageMask = waitStages; // Stages to check semaphores at

        //// Only submit the command buffer we want to render to since they're 1:1 with SwapChain Images
        //submitInfo.commandBufferCount = 1;
        //submitInfo.pCommandBuffers = &commandList->commandBuffers[viewport->currentImageIndex];

        //// Semaphores to signal when command buffer finishes executing (finished rendering)
        //submitInfo.signalSemaphoreCount = 1;
        //submitInfo.pSignalSemaphores = &commandList->renderFinishedSemaphore[viewport->currentImageIndex]; // 1:1 with frames (NumCommandBuffers)

        //result = vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &submitInfo, commandList->renderFinishedFence[viewport->currentImageIndex]);
		
        return true;
    }

    bool VulkanRHI::PresentViewport(RHI::GraphicsCommandList* viewportCommandList)
    {
  //      auto vulkanCommandList = (GraphicsCommandList*)viewportCommandList;
  //      if (!vulkanCommandList->IsViewportTarget() || vulkanCommandList->viewport == nullptr)
  //          return false;

  //      auto vulkanViewport = vulkanCommandList->GetViewport();
		//VkSwapchainKHR swapChain = nullptr;// vulkanViewport->GetSwapChain()->GetHandle();
		
  //      VkPresentInfoKHR presentInfo{};
  //      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  //      presentInfo.waitSemaphoreCount = 1;
  //      presentInfo.pWaitSemaphores = &vulkanCommandList->renderFinishedSemaphore[vulkanViewport->currentImageIndex];
  //      presentInfo.swapchainCount = 1;
  //      presentInfo.pSwapchains = &swapChain;
  //      presentInfo.pImageIndices = &vulkanViewport->currentImageIndex;

  //      auto result = vkQueuePresentKHR(device->GetGraphicsQueue()->GetHandle(), &presentInfo);
  //      if (result != VK_SUCCESS)
  //          return false;

  //      // Next frame index (if we're rendering 2 frames simultaneously for triple buffering)
  //      vulkanViewport->currentDrawFrameIndex = (vulkanViewport->currentDrawFrameIndex + 1) % vulkanViewport->GetSimultaneousFrameDrawCount();

        return true;
    }

    // - Resources -

	RHI::SwapChain* VulkanRHI::CreateSwapChain(PlatformWindow* window, const RHI::SwapChainDescriptor& desc)
	{
		return new Vulkan::SwapChain(this, device, window, desc);
	}

	void VulkanRHI::DestroySwapChain(RHI::SwapChain* swapChain)
	{
		delete swapChain;
	}

	RHI::MemoryHeap* VulkanRHI::AllocateMemoryHeap(const MemoryHeapDescriptor& desc)
	{
		return new MemoryHeap(device, desc);
	}

	void VulkanRHI::FreeMemoryHeap(RHI::MemoryHeap* memoryHeap)
	{
		delete memoryHeap;
	}

	RHI::Buffer* VulkanRHI::CreateBuffer(const RHI::BufferDescriptor& bufferDesc)
    {
        return new Buffer(device, bufferDesc);
    }

	RHI::Buffer* VulkanRHI::CreateBuffer(const BufferDescriptor& bufferDesc, const ResourceMemoryDescriptor& memoryDesc)
	{
		return new Buffer(device, bufferDesc, memoryDesc);
	}

    void VulkanRHI::DestroyBuffer(RHI::Buffer* buffer)
    {
        delete buffer;
    }

    RHI::Texture* VulkanRHI::CreateTexture(const RHI::TextureDescriptor& textureDesc)
    {
        return new Texture(device, textureDesc);
    }

	RHI::Texture* VulkanRHI::CreateTexture(const TextureDescriptor& textureDesc, const ResourceMemoryDescriptor& memoryDesc)
	{
		return new Texture(device, textureDesc, memoryDesc);
	}

    void VulkanRHI::DestroyTexture(RHI::Texture* texture)
    {
        delete texture;
    }

    RHI::Sampler* VulkanRHI::CreateSampler(const RHI::SamplerDescriptor& samplerDesc)
    {
        return new Sampler(device, samplerDesc);
    }

    void VulkanRHI::DestroySampler(RHI::Sampler* sampler)
    {
        delete sampler;
    }

    void* VulkanRHI::AddImGuiTexture(RHI::Texture* texture, RHI::Sampler* sampler)
    {
        return VulkanPlatform::AddImGuiTexture((Texture*)texture, (Sampler*)sampler);
    }

    void VulkanRHI::RemoveImGuiTexture(void* imguiTexture)
    {
		device->WaitUntilIdle();
        VulkanPlatform::RemoveImGuiTexture((VkDescriptorSet)imguiTexture);
    }

	RHI::ShaderModule* VulkanRHI::CreateShaderModule(const RHI::ShaderModuleDescriptor& desc)
	{
		return new Vulkan::ShaderModule(device, desc);
	}

	void VulkanRHI::DestroyShaderModule(RHI::ShaderModule* shaderModule)
	{
		delete shaderModule;
	}

	RHI::ShaderResourceGroup* VulkanRHI::CreateShaderResourceGroup(const RHI::ShaderResourceGroupLayout& srgLayout)
	{
		return new Vulkan::ShaderResourceGroup(device, srgLayout);
	}

	void VulkanRHI::DestroyShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup)
	{
		delete shaderResourceGroup;
	}

    RHI::PipelineState* VulkanRHI::CreateGraphicsPipeline(const RHI::GraphicsPipelineDescriptor& desc)
    {
        return new Vulkan::PipelineState(device, desc);
    }

    void VulkanRHI::DestroyPipeline(const RHI::PipelineState* pipeline)
    {
        delete pipeline;
    }

	Array<RHI::CommandQueue*> VulkanRHI::GetHardwareQueues(RHI::HardwareQueueClassMask queueMask)
	{
		return device->GetHardwareQueues(queueMask);
	}

} // namespace CE
