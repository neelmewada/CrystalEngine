#pragma once

#include "VulkanRHIPrivate.h"

#include "vulkan/vulkan.h"

namespace CE {
	class PlatformWindow;
}

namespace CE::Vulkan
{
    class VulkanDevice;
    class Texture;

    struct VulkanSwapChainImage
    {
        VkImage image = nullptr;
        VkImageView imageView = nullptr;
    };

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain(VulkanRHI* vulkanRHI, PlatformWindow* windowHandle, VulkanDevice* device,
            u32 desiredBackBufferCount, u32 simultaneousFrameDraws,
            u32 width, u32 height, bool isFullscreen, 
            RHI::ColorFormat colorFormat = RHI::ColorFormat::Auto,
            RHI::DepthStencilFormat depthBufferFormat = RHI::DepthStencilFormat::Auto);
        virtual ~VulkanSwapChain();

        // - Getters -

        CE_INLINE u32 GetWidth() { return width; }
        CE_INLINE u32 GetHeight() { return height; }

        CE_INLINE u32 GetBackBufferCount() { return backBufferCount; }
        CE_INLINE u32 GetSimultaneousFrameDraws() { return simultaneousFrameDraws; }

        CE_INLINE VkSwapchainKHR GetHandle() { return swapChain; }

        CE_INLINE bool HasDepthStencilAttachment() { return depthBufferFormat != RHI::DepthStencilFormat::None; }

        void RebuildSwapChain();

    protected:

        void CreateSurface();
        void CreateSwapChain();
        void CreateDepthBuffer();

        void DestroyDepthBuffer();

    private:
        VulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        PlatformWindow* windowHandle = nullptr;
        bool isFullscreen = false;

        VkSwapchainKHR swapChain = nullptr;
        VkSurfaceKHR surface = nullptr;

        VkPresentModeKHR presentMode{};
        VkSurfaceFormatKHR swapChainColorFormat{};
        VkFormat swapChainDepthFormat = VK_FORMAT_UNDEFINED;

        RHI::ColorFormat colorBufferFormat{};
        RHI::DepthStencilFormat depthBufferFormat{};

        u32 backBufferCount = 0;
        u32 simultaneousFrameDraws = 0;
        u32 width = 0;
        u32 height = 0;

        Array<VulkanSwapChainImage> swapChainColorImages{};
        Texture* swapChainDepthImage{};

        friend struct VulkanRenderTargetLayout;
        friend class VulkanFrameBuffer;
        friend class Viewport;
    };

} // namespace CE
