
#include "PAL/Common/VulkanPlatform.h"

#include "VulkanRHIPrivate.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

namespace CE::Vulkan
{
    VkDescriptorSet VulkanPlatformBase::AddImGuiTexture(Texture* texture, VulkanSampler* sampler)
    {
        if (texture == nullptr)
            return nullptr;
        return ImGui_ImplVulkan_AddTexture(sampler->GetVkHandle(), texture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void VulkanPlatformBase::RemoveImGuiTexture(VkDescriptorSet imguiTexture)
    {
        ImGui_ImplVulkan_RemoveTexture(imguiTexture);
    }

} // namespace CE

