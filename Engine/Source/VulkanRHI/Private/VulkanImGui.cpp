#pragma once

#include "CoreApplication.h"

#include "VulkanRHIPrivate.h"

#include "VulkanRenderPass.h"
#include "VulkanViewport.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"

#include "PAL/Common/VulkanPlatform.h"

#define IMGUI_DEFINE_MATH_OPERATORS

// Explort ImGui Impl
#define IMGUI_API DLL_EXPORT

#include "imgui.h"
#if PAL_TRAIT_SDL_SUPPORTED
#include "backends/imgui_impl_sdl2.h"
#endif
#include "backends/imgui_impl_vulkan.h"

#include "misc/cpp/imgui_stdlib.h"

namespace CE
{

    bool VulkanGraphicsCommandList::InitImGui(RHI::FontPreloadConfig* preloadFontConfig, Array<void*>& outFontHandles)
    {
        VkResult result = VK_SUCCESS;
		
        // Create Descriptor Pool
        {
            VkDescriptorPoolSize poolSizes[] =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };
            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
            poolInfo.poolSizeCount = (u32)IM_ARRAYSIZE(poolSizes);
            poolInfo.pPoolSizes = poolSizes;
            auto result = vkCreateDescriptorPool(device->GetHandle(), &poolInfo, nullptr, &imGuiDescriptorPool);

            if (result != VK_SUCCESS)
            {
                CE_LOG(Error, All, "Failed to create descriptor pool for ImGui. Error: {}", (int)result);
                return false;
            }
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
		
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        if (preloadFontConfig != nullptr && preloadFontConfig->preloadFontCount > 0 && preloadFontConfig->preloadFonts != nullptr)
        {
            for (int i = 0; i < preloadFontConfig->preloadFontCount; i++)
            {
                auto numBytes = preloadFontConfig->preloadFonts[i].byteSize;
                char* fontData = new char[numBytes];
                memcpy(fontData, preloadFontConfig->preloadFonts[i].fontData, numBytes);
				if (preloadFontConfig->preloadFonts[i].isCompressed)
				{
					void* fontHandle = io.Fonts->AddFontFromMemoryCompressedTTF(fontData, numBytes, preloadFontConfig->preloadFonts[i].pointSize);
					outFontHandles.Add(fontHandle);
				}
				else
				{
					void* fontHandle = io.Fonts->AddFontFromMemoryTTF(fontData, numBytes, preloadFontConfig->preloadFonts[i].pointSize);
					outFontHandles.Add(fontHandle);
				}
            }
        }

        ImGui::StyleColorsDark();

        auto& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ASSERT(IsViewportTarget(), "ImGui is only supported on viewport render targets at this moment!");

        VulkanPlatform::InitVulkanForWindow(viewport->GetWindowHandle());

        gPersistentRenderPass = viewport->renderTarget->renderPass->GetHandle();

        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.PhysicalDevice = device->GetPhysicalHandle();
        initInfo.Device = device->GetHandle();
        initInfo.DescriptorPool = imGuiDescriptorPool;
        initInfo.Instance = vulkanRHI->GetInstanceHandle();
        initInfo.ImageCount = viewport->swapChain->GetBackBufferCount();
        initInfo.MinImageCount = 2;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Queue = device->GetGraphicsQueue()->GetHandle();
        initInfo.QueueFamily = device->GetGraphicsQueue()->GetFamilyIndex();
        ImGui_ImplVulkan_Init(&initInfo, gPersistentRenderPass);

        // Upload fonts
        {
            VkCommandPool cmdPool = device->GetGraphicsCommandPool();
            VkCommandBuffer cmdBuffer = commandBuffers[viewport->currentDrawFrameIndex];

            vkResetCommandPool(device->GetHandle(), cmdPool, 0);
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer(cmdBuffer, &beginInfo);

            ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);

            VkSubmitInfo endInfo = {};
            endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            endInfo.commandBufferCount = 1;
            endInfo.pCommandBuffers = &cmdBuffer;
            vkEndCommandBuffer(cmdBuffer);
            vkQueueSubmit(device->GetGraphicsQueue()->GetHandle(), 1, &endInfo, VK_NULL_HANDLE);

            vkDeviceWaitIdle(device->GetHandle());
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

        isImGuiEnabled = true;

        PlatformApplication::Get()->AddMessageHandler(this);
        return true;
    }

    void VulkanGraphicsCommandList::ShutdownImGui()
    {
        PlatformApplication::Get()->RemoveMessageHandler(this);

        vkDeviceWaitIdle(device->GetHandle());

        ImGui_ImplVulkan_Shutdown();
        VulkanPlatform::ShutdownVulkanForWindow();
        ImGui::DestroyContext();

        // Destroy Descriptor Pool
        vkDestroyDescriptorPool(device->GetHandle(), imGuiDescriptorPool, nullptr);
        imGuiDescriptorPool = nullptr;

        gPersistentRenderPass = nullptr;
        isImGuiEnabled = false;
    }

    void VulkanGraphicsCommandList::ImGuiNewFrame()
    {
        ImGui_ImplVulkan_NewFrame();
		if (IsViewportTarget())
			ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanGraphicsCommandList::ImGuiRender()
    {
        ImGui::Render();
        ImDrawData* imGuiDrawData = ::ImGui::GetDrawData();

        for (int i = 0; i < commandBuffers.GetSize(); i++)
        {
            ImGui_ImplVulkan_RenderDrawData(imGuiDrawData, commandBuffers[i]);
        }
    }

    void VulkanGraphicsCommandList::ImGuiPlatformUpdate()
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void VulkanGraphicsCommandList::ProcessNativeEvents(void* nativeEvent)
    {
        if (isImGuiEnabled)
        {
#if PAL_TRAIT_SDL_SUPPORTED
            ImGui_ImplSDL2_ProcessEvent((SDL_Event*)nativeEvent);
#endif
        }
    }
    
} // namespace CE


#include "imgui.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_tables.cpp"
#include "imgui_widgets.cpp"

#if PAL_TRAIT_SDL_SUPPORTED
#include "backends/imgui_impl_sdl2.cpp"
#endif
#include "backends/imgui_impl_vulkan.cpp"

#include "misc/cpp/imgui_stdlib.cpp"
