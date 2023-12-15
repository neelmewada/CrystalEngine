#pragma once

#include "CoreMinimal.h"
#include "CoreApplication.h"
#include "CoreRHI.h"
#include "CoreShader.h"

#include "VulkanRHI.h"
#include "VulkanDevice.h"

#include "vulkan/vulkan.h"

namespace CE
{
    class VulkanViewport;
	class VulkanDescriptorSet;
	class VulkanShaderResourceGroup;
	class VulkanShaderResourceManager;
	class VulkanPipelineLayout;

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

        Color clearColors[RHI::MaxSimultaneousRenderOutputs] = {};

        VkFormat colorFormats[RHI::MaxSimultaneousRenderOutputs] = {};
        VkFormat depthFormat = {};

        VkAttachmentReference colorReferences[RHI::MaxSimultaneousRenderOutputs] = {};
        VkAttachmentReference depthStencilReference = {};

        VkAttachmentDescription attachmentDesc[RHI::MaxSimultaneousRenderOutputs + 1]; // countof(ColorAttachments) + 1 depth attachment

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

    /*
    *   Vulkan Frame Buffer
    */

    class VulkanSwapChain;
    class VulkanRenderTarget;
    class VulkanTexture;

    class VulkanFrameBuffer
    {
    public:
        VulkanFrameBuffer(VulkanDevice* device, VulkanSwapChain* swapChain, u32 swapChainImageIndex, VulkanRenderTarget* renderTarget);
        VulkanFrameBuffer(VulkanDevice* device, VkImageView attachments[RHI::MaxSimultaneousRenderOutputs + 1], VulkanRenderTarget* renderTarget);

        ~VulkanFrameBuffer();

        inline VkFramebuffer GetHandle() const
        {
            return frameBuffer;
        }

    private:
        VulkanDevice* device = nullptr;
        VkFramebuffer frameBuffer = nullptr;
    };

    // Forward decls

    class VulkanRenderPass;
	class VulkanSampler;
	class Viewport;

    struct VulkanFrame
    {
        List<VulkanTexture*> textures{};
        //List<VkSampler> samplers{};
		List<VulkanSampler*> samplers{};
        VulkanFrameBuffer* framebuffer = nullptr;
    };
    
    /// Vulkan Render Target class
    class VulkanRenderTarget : public RHI::RenderTarget
    {
    public:
        /// An offscreen render target
        VulkanRenderTarget(VulkanDevice* device, const VulkanRenderTargetLayout& rtLayout);

        /// A viewport render target
        VulkanRenderTarget(VulkanDevice* device, VulkanViewport* viewport, const VulkanRenderTargetLayout& rtLayout);

        virtual ~VulkanRenderTarget();

        virtual bool IsViewportRenderTarget() override { return isViewportRenderTarget; }

		virtual RHI::Viewport* GetRenderTargetViewport() override;

        virtual RHI::RenderPass* GetRenderPass() override;

        virtual void SetClearColor(u32 colorTargetIndex, const Color& color) override;

		virtual void Resize(u32 newWidth, u32 newHeight) override;

		virtual int GetNumColorBuffers() override
		{
			return colorFrames.GetSize();
		}

		virtual RHI::Texture* GetColorTargetTexture(int index, int attachmentIndex = 0) override;
		virtual RHI::Sampler* GetColorTargetTextureSampler(int index, int attachmentIndex = 0) override;

        // - Getters -

        CE_INLINE VulkanRenderPass* GetVulkanRenderPass() { return renderPass; }

        CE_INLINE VulkanViewport* GetVulkanViewport() { return viewport; }

        VkRenderPass GetVulkanRenderPassHandle() const;

        u32 GetBackBufferCount();

        u32 GetSimultaneousFrameDrawCount();

        virtual u32 GetWidth() override;

        virtual u32 GetHeight() override;

        u32 GetColorAttachmentCount()
        {
            return rtLayout.colorAttachmentCount;
        }

        u32 GetTotalAttachmentCount()
        {
            return rtLayout.attachmentDescCount;
        }

        bool HasDepthStencilAttachment() const
        {
            return rtLayout.HasDepthStencilAttachment();
        }


    protected:
        void CreateDepthBuffer();
        void DestroyDepthBuffer();
        void CreateColorBuffers();
        void DestroyColorBuffers();

    private:
        bool isViewportRenderTarget = false;
        VulkanViewport* viewport = nullptr;
        bool isFresh = true;
        
        Color clearColors[RHI::MaxSimultaneousRenderOutputs] = {};
        VulkanDevice* device = nullptr;
        VulkanRenderTargetLayout rtLayout{};
        VulkanRenderPass* renderPass = nullptr;
        u32 backBufferCount = 0; 
        u32 simultaneousFrameDraws = 0;

        u32 width = 0, height = 0;

        u32 currentDrawFrameIndex = 0;
        u32 currentImageIndex = 0;

        // Offscreen render targets
        Array<VulkanFrame> colorFrames{};
        VulkanFrame depthFrame{};

        friend class VulkanViewport;
        friend class VulkanGraphicsCommandList;
        friend class VulkanFrameBuffer;
        friend class VulkanRHI;
		friend class VulkanGraphicsPipeline;
    };

    /*
    *   Graphics Command List
    */

    class VulkanGraphicsCommandList : public RHI::GraphicsCommandList, public ApplicationMessageHandler
    {
    public:
        VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanViewport* viewport);
        VulkanGraphicsCommandList(VulkanRHI* vulkanRHI, VulkanDevice* device, VulkanRenderTarget* renderTarget);
        virtual ~VulkanGraphicsCommandList();

        // - Command List API -

        virtual void Begin() override;
        virtual void End() override;

		virtual void SetScissorRects(u32 scissorCount, const RHI::ScissorState* scissors) override;
		virtual void SetViewportRects(u32 viewportCount, const RHI::ViewportState* viewports) override;

		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers) override;
		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets) override;

		virtual void BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset) override;

		virtual void BindPipeline(RHI::IPipelineState* pipeline) override;

		virtual void CommitShaderResources(const Array<RHI::ShaderResourceGroup*>& shaderResourceGroups) override;
		
		// TODO: Re-implement this function with more flexibilty. Maybe expose Fences?
		virtual void WaitForExecution() override;

		virtual void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance) override;

		// - Getters -

        CE_INLINE bool IsViewportTarget()
        {
            return viewport != nullptr;
        }

        CE_INLINE VulkanViewport* GetViewport()
        {
            return viewport;
        }

        CE_INLINE VulkanRenderTarget* GetRenderTarget()
        {
            return renderTarget;
        }

        void ProcessNativeEvents(void* nativeEvent) override;

        // - ImGui API -

        bool InitImGui(RHI::FontPreloadConfig* preloadFonts, Array<void*>& outFontHandles) override;

        void ShutdownImGui() override;

        virtual void ImGuiNewFrame() override;
        virtual void ImGuiRender() override;

        void ImGuiPlatformUpdate() override;

    protected:
        void CreateSyncObjects();
        void DestroySyncObjects();

    private:
        VulkanRHI* vulkanRHI = nullptr;
        VulkanDevice* device = nullptr;
        VulkanRenderTarget* renderTarget = nullptr;
        VulkanViewport* viewport = nullptr;
		VulkanShaderResourceManager* srgManager = nullptr;
		VulkanPipelineLayout* currentPipelineLayout = nullptr;

        u32 numCommandBuffers = 1; // = BackBufferCount
        u32 simultaneousFrameDraws = 1;

        u32 currentImageIndex = 0;

        List<VkCommandBuffer> commandBuffers{};

        List<VkFence> renderFinishedFence{}; // Size = NumCommandBuffers
        List<VkSemaphore> renderFinishedSemaphore{}; // Size = NumCommandBuffers

		HashMap<Name, VulkanShaderResourceGroup*> boundSRGs{};
		HashMap<int, Array<VulkanShaderResourceGroup*>> boundSRGsBySetNumber{};
		HashMap<int, VulkanShaderResourceGroup*> boundMainSRGBySetNumber{};
		HashSet<int> modifiedDescriptorSetNumbers{};

        // ImGui
        b8 isImGuiEnabled = false;
        VkDescriptorPool imGuiDescriptorPool;

        friend class VulkanRHI;

    };

} // namespace CE
