#pragma once

#include "VulkanRHIPrivate.h"

#include "vulkan/vulkan.h"

namespace CE
{
    class VulkanDevice;
    class VulkanTexture;

    struct VulkanSwapChainImage
    {
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
    };

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(NuklearVulkanRHI* vulkanRHI, void* windowHandle, VulkanDevice* device,
            u32 desiredBackBufferCount, u32 simultaneousFrameDraws,
            u32 width, u32 height, bool isFullscreen, 
            RHIColorFormat colorFormat = RHIColorFormat::Auto, 
            RHIDepthStencilFormat depthBufferFormat = RHIDepthStencilFormat::Auto);
        virtual ~VulkanSwapChain();

        // - Getters -

        CE_INLINE u32 GetWidth() { return width; }
        CE_INLINE u32 GetHeight() { return height; }

        CE_INLINE u32 GetBackBufferCount() { return backBufferCount; }
        CE_INLINE u32 GetSimultaneousFrameDraws() { return simultaneousFrameDraws; }

        CE_INLINE VkSwapchainKHR GetHandle() { return swapChain; }

        CE_INLINE bool HasDepthStencilAttachment() { return depthBufferFormat != RHIDepthStencilFormat::None; }

    protected:

        void CreateSurface();
        void CreateSwapChain();
        void CreateDepthBuffer();

        void DestroyDepthBuffer();

    private:
        NuklearVulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        void* windowHandle = nullptr;
        bool isFullscreen = false;

        VkSwapchainKHR swapChain = nullptr;
        VkSurfaceKHR surface = nullptr;

        VkPresentModeKHR presentMode{};
        VkSurfaceFormatKHR swapChainColorFormat{};
        VkFormat swapChainDepthFormat = VK_FORMAT_UNDEFINED;

        RHIColorFormat colorBufferFormat{};
        RHIDepthStencilFormat depthBufferFormat{};

        u32 backBufferCount = 0;
        u32 simultaneousFrameDraws = 0;
        u32 width = 0;
        u32 height = 0;

        Array<VulkanSwapChainImage> swapChainColorImages{};
        VulkanTexture* swapChainDepthImage{};

        friend struct VulkanRenderTargetLayout;
        friend class VulkanFrameBuffer;
        friend class VulkanViewport;
    };

} // namespace CE
