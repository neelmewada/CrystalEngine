#pragma once

#include "Rendering/IGraphicsPipelineState.h"

#include "DeviceContextVk.h"
#include "SwapChainVk.h"
#include "RenderContextVk.h"
#include "ShaderVk.h"
#include "ShaderResourceBindingVk.h"

#include <vulkan/vulkan.h>

#include <array>

namespace Vox
{

class RenderContextVk;

class GraphicsPipelineStateVk : public IGraphicsPipelineState
{
public:
    friend class ShaderResourceBindingVk;
    GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo& createInfo);
    ~GraphicsPipelineStateVk();

public:
    DELETE_COPY_CONSTRUCTORS(GraphicsPipelineStateVk)

public: // Public API
    IShaderResourceVariable* GetStaticVariableByName(const char* pName) override;
    IShaderResourceBinding* GetShaderResourceBinding() override { return m_pBinding; }

    IShaderResourceBinding* CreateResourceBinding(ResourceBindingFrequency bindingFrequency) override;

    void CmdBindDescriptorSets(VkCommandBuffer commandBuffer);

    // - Getters
    VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
    VkPipeline GetPipeline() { return m_Pipeline; }

public: // Shader Resource Binding Callbacks

private: // Internal API
    void CreateImmutableSamplers(const GraphicsPipelineStateCreateInfo& createInfo);
    void CreateDescriptorSetLayouts(const GraphicsPipelineStateCreateInfo& createInfo);
    void CreateResourceBindings(const GraphicsPipelineStateCreateInfo& createInfo);
    void CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo);

    VkFormat VkFormatFromVertexAttribFormat(VertexAttribFormat& attribFormat);

private: // Internal Members
    // - References
    DeviceContextVk* m_pDevice = nullptr;
    SwapChainVk* m_pSwapChain = nullptr;
    ShaderVk* m_pShader = nullptr;
    RenderContextVk* m_pRenderContext = nullptr;
    IShaderResourceBinding* m_pStaticBinding = nullptr;
    IShaderResourceBinding* m_pBinding = nullptr;

    // - Data
    std::string m_Name;

private: // Vulkan Members
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkPipeline m_Pipeline = nullptr;
    VkDescriptorPool m_StaticDescriptorPool = nullptr;
    VkDescriptorPool m_DescriptorPool = nullptr;

    std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts{};
    std::map<Uint32, std::vector<VkDescriptorSet>> m_DescriptorSetsPerFrame{};
    std::map<std::string, VkSampler> m_ImmutableSamplers{};

    std::map<ResourceLocation, BufferVk*> m_DynamicOffsetBuffers{};
    std::map<ResourceLocation, ShaderResourceVariableDefinition> m_ShaderVariableDefinitions;
    std::map<ResourceLocation, ShaderResourceVariableDesc> m_ShaderVariables;
};

}
