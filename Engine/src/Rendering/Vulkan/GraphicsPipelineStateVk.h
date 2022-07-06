#pragma once

#include "Rendering/IGraphicsPipelineState.h"

#include "DeviceContextVk.h"
#include "SwapChainVk.h"
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
    GraphicsPipelineStateVk(const GraphicsPipelineStateVk&) = delete;

public: // Public API
    // - Getters
    VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
    VkPipeline GetPipeline() { return m_Pipeline; }
    VkDescriptorSet GetDescriptorSet(int frameIndex) {
        if (frameIndex >= m_DescriptorSets.size()) return nullptr;
        return m_DescriptorSets[frameIndex];
    }

private: // Internal API
    void CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo);
    // TODO: Temp function
    void CreateUniformBuffer(uint64_t bufferSize, BufferData& initialData) override;
    void UpdateUniformBuffer(BufferData& data) override;
    VkFormat VkFormatFromVertexAttribFormat(VertexAttribFormat& attribFormat);

private: // Internal Members
    // - References
    DeviceContextVk* m_pDevice = nullptr;
    SwapChainVk* m_pSwapChain = nullptr;
    ShaderVk* m_pShader = nullptr;

private: // Vulkan Members
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkPipeline m_Pipeline = nullptr;
    VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;

    std::vector<BufferVk*> m_UniformBuffer{};
    std::vector<VkDescriptorSet> m_DescriptorSets;
};

}
