#pragma once

#include "Rendering/EngineFactoryVk.h"
#include "Rendering/ISwapChain.h"

#include "EngineContextVk.h"
#include "DeviceContextVk.h"

namespace Vox
{

struct SwapchainImage
{
    VkImage image;
    VkImageView imageView;
};

class SwapChainVk : public ISwapChain
{
private:
    friend class EngineFactoryVk;
    friend class RenderContextVk;
    SwapChainVk(SwapChainCreateInfoVk& swapChainInfo);
    ~SwapChainVk();

public: // Public API
    // - API
    void Submit() override;
    void Present() override;
    void Resize() override;
    void GetSize(uint32_t *w, uint32_t *h) override {
        *w = m_SwapchainExtent.width;
        *h = m_SwapchainExtent.height;
    }

    // - Getters
    VkExtent2D GetExtent() { return m_SwapchainExtent; }
    VkRenderPass GetRenderPass() { return m_RenderPass; }
    VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }
    int GetCurrentFrameIndex() { return m_CurrentFrameIndex; }
    int GetMaxSimultaneousFrameDraws() { return m_MaxSimultaneousFrameDraws; }
    const std::vector<VkCommandBuffer>& GetCommandBuffers() { return m_CommandBuffers; }
    const std::vector<VkFramebuffer>& GetFramebuffers() { return m_SwapchainFramebuffers; }

    // - Setters
    void SetRenderContext(IRenderContext* pRenderContext) { m_pRenderContext = pRenderContext; }

private: // Internal API
    // - Vulkan Main
    void CreateSwapChain();
    void CreateRenderPass();
    void CreateDepthBufferImage();
    void CreateFramebuffers();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    void CreateDescriptorPool(); // TODO: Temp Function

    void RecreateSwapChainObjects();

    // - Helpers
    VkImage CreateImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VmaAllocation* pAllocation);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);
    VkFormat ChooseDepthFormat();

private: // Internal Members
    int m_CurrentFrameIndex = 0; // Index of current frame we're rendering
    uint32_t m_CurrentImageIndex = 0; // Index of current swapchain image we're rendering to
    int m_MaxSimultaneousFrameDraws = 0;
    EngineContextVk* m_pEngine = nullptr;
    DeviceContextVk* m_pDevice = nullptr;
    IRenderContext* m_pRenderContext = nullptr;

private: // Vulkan Members
    VkSwapchainKHR m_Swapchain = nullptr;
    VkRenderPass m_RenderPass = nullptr;
    VkDescriptorPool m_DescriptorPool = nullptr;

    // - Swapchain details
    VkFormat m_SwapchainImageFormat{};
    VkExtent2D m_SwapchainExtent{};

    // - Swapchain & Commands
    std::vector<SwapchainImage> m_SwapchainImages;
    std::vector<VkFramebuffer> m_SwapchainFramebuffers;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    // - Depth Buffer
    VmaAllocation m_DepthImageAllocation = nullptr;
    VkImage m_DepthBufferImage = nullptr;
    VkImageView m_DepthBufferImageView = nullptr;

    // - Synchronization
    std::vector<VkSemaphore> m_ImageAvailableForRendering;
    std::vector<VkSemaphore> m_RenderFinished;
    std::vector<VkFence> m_DrawFinishedFences;
};

}
