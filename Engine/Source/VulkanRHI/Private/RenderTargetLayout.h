
namespace CE::Vulkan
{

    struct VulkanRenderTargetLayout
    {
    public:
        VulkanRenderTargetLayout() = default;
        ~VulkanRenderTargetLayout() = default;

        /// Offscreen render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, u32 width, u32 height, const RHI::RenderTargetLayout& rtLayout);

        /// Viewport render target layout
        VulkanRenderTargetLayout(VulkanDevice* device, VulkanViewport* viewport, const RHI::RenderTargetLayout& rtLayout);

        u32 width = 0, height = 0;
        u32 presentationRTIndex = -1;

        Color clearColors[RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight] = {};

        VkFormat colorFormats[RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight] = {};
        VkFormat depthFormat = {};

        VkAttachmentReference colorReferences[RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight] = {};
        VkAttachmentReference depthStencilReference = {};

        VkAttachmentDescription attachmentDesc[RHI::Limits::Pipeline::MaxSimultaneousFramesInFlight + 1]; // countof(ColorAttachments) + 1 depth attachment

        u8 colorAttachmentCount = 0;

        // colorAttachmentCount + 1 depth attachment (if exists)
        u8 attachmentDescCount = 0;

        u32 backBufferCount = 2, simultaneousFrameDraws = 1;

        CE_INLINE bool HasDepthStencilAttachment() const
        {
            return hasDepthStencilAttachment;
        }

        CE_INLINE bool IsValid() const
        {
            return isValid;
        }

    private:
        bool isValid = false;
        bool hasDepthStencilAttachment = false;

    };
    
} // namespace CE::Vulkan

