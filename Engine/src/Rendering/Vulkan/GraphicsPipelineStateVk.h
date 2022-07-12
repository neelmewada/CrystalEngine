#pragma once

#include "Rendering/IGraphicsPipelineState.h"

#include "DeviceContextVk.h"
#include "SwapChainVk.h"
#include "RenderContextVk.h"
#include "ShaderVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class GraphicsPipelineStateVk : public IGraphicsPipelineState
{
public:
    GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo& createInfo);
    ~GraphicsPipelineStateVk();

public:
    DELETE_COPY_CONSTRUCTORS(GraphicsPipelineStateVk)

public: // Public API
    // TODO: All of them are Temp functions. To be changed.
    void CreateUniformBuffer(uint64_t bufferSize, BufferData& initialData) override;
    void UpdateUniformBuffer(BufferData& data) override;
    void CreateDynamicUniformBuffer(Uint64 bufferSize, BufferData& initialData, uint64_t alignment) override;
    void UpdateDynamicUniformBuffer(BufferData& data) override;

    // - Getters
    VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
    VkPipeline GetPipeline() { return m_Pipeline; }
    uint64_t GetDynamicUniformBufferAlignment() { return m_DynamicUniformBufferAlignment; }
    VkDescriptorSet GetDescriptorSet(int frameIndex) {
        if (frameIndex >= m_DescriptorSets.size()) return nullptr;
        return m_DescriptorSets[frameIndex];
    }

private: // Internal API
    void CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo);

    VkFormat VkFormatFromVertexAttribFormat(VertexAttribFormat& attribFormat);

private: // Internal Members
    // - References
    DeviceContextVk* m_pDevice = nullptr;
    SwapChainVk* m_pSwapChain = nullptr;
    ShaderVk* m_pShader = nullptr;
    RenderContextVk* m_pRenderContext = nullptr;

    // - Info
    uint64_t m_DynamicUniformBufferAlignment = 0;

private: // Vulkan Members
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkPipeline m_Pipeline = nullptr;
    //VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;

    std::vector<BufferVk*> m_VPUniformBuffer{};
    std::vector<BufferVk*> m_ModelUniformBufferDynamic{};
    std::vector<VkDescriptorSet> m_DescriptorSets;
};

}
