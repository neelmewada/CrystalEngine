
#include "VulkanRHIPrivate.h"

namespace CE
{
    // ****************************************************
    // VulkanRenderTargetLayout

    VulkanRenderTargetLayout::VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHIRenderTargetLayout& rtLayout)
    {
        this->width = width;
        this->height = height;


    }

    // ****************************************************
    // VulkanRenderTarget

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, u32 backBufferCount, u32 simultaneousFrameDraws, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRT(false)
    {

    }

    VulkanRenderTarget::VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout)
        : isViewportRT(true)
    {

    }

    VulkanRenderTarget::~VulkanRenderTarget()
    {

    }

} // namespace CE

