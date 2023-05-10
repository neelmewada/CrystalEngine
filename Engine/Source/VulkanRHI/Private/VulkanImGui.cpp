#pragma once

#include "CoreApplication.h"

#include "VulkanRHIPrivate.h"

#include "VulkanRenderPass.h"
#include "VulkanViewport.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture.h"

#include "PAL/Common/VulkanPlatform.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#if PAL_TRAIT_SDL_SUPPORTED
#include "backends/imgui_impl_sdl2.h"
#endif
#include "backends/imgui_impl_vulkan.h"

namespace CE
{

    bool VulkanGraphicsCommandList::InitImGui(IMGUIFontPreloadConfig* preloadFonts)
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
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        ImGui::StyleColorsDark();

        auto& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        if (IsViewportTarget())
        {
            VulkanPlatform::InitVulkanForWindow(viewport->GetWindowHandle());
        }

        isImGuiEnabled = true;
        return true;
    }

    void VulkanGraphicsCommandList::ShutdownImGui()
    {
        if (IsViewportTarget())
        {
            VulkanPlatform::ShutdownVulkanForWindow();
        }

        // Destroy Descriptor Pool
        vkDestroyDescriptorPool(device->GetHandle(), imGuiDescriptorPool, nullptr);
        imGuiDescriptorPool = nullptr;

        isImGuiEnabled = false;
    }
    
} // namespace CE


// ImGui Impl
#include "imgui.cpp"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_tables.cpp"
#include "imgui_widgets.cpp"
#include "backends/imgui_impl_sdl2.cpp"
#include "backends/imgui_impl_vulkan.cpp"
