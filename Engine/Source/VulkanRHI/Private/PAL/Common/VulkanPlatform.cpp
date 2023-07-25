
#include "PAL/Common/VulkanPlatform.h"

#include "VulkanRHIPrivate.h"
#include "VulkanTexture.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

namespace CE
{
    VkDescriptorSet VulkanPlatformBase::AddImGuiTexture(VulkanTexture* texture)
    {
        // TODO: Add sampler
        return ImGui_ImplVulkan_AddTexture(nullptr, texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
} // namespace CE

