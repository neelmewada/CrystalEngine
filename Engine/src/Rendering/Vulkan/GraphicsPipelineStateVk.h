#pragma once

#include "Rendering/IGraphicsPipelineState.h"

#include "DeviceContextVk.h"
#include "SwapChainVk.h"
#include "RenderContextVk.h"
#include "ShaderVk.h"
#include "ShaderResourceBindingVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class GraphicsPipelineStateVk : public IGraphicsPipelineState, public IShaderResourceBindingCallbacks
{
public:
    GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo& createInfo);
    ~GraphicsPipelineStateVk();

public:
    DELETE_COPY_CONSTRUCTORS(GraphicsPipelineStateVk)

public: // Public API
    IShaderResourceVariable* GetStaticVariableByName(const char* pName) override;
    IShaderResourceBinding* GetShaderResourceBinding() override { return m_Binding; }

    void CmdBindDescriptorSets(VkCommandBuffer commandBuffer);

    // - Getters
    VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }
    VkPipeline GetPipeline() { return m_Pipeline; }

public: // Shader Resource Binding Callbacks
    void BindShaderResource(IShaderResourceBinding* resourceBinding, IDeviceObject *pDeviceObject, Uint32 set, Uint32 binding,
                            Uint32 descriptorCount, ShaderResourceVariableType resourceType) override;

private: // Internal API
    void CreateDescriptorSets(const GraphicsPipelineStateCreateInfo& createInfo);
    void CreateResourceBindings(const GraphicsPipelineStateCreateInfo& createInfo);
    void CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo);

    void BuildDynamicOffsets();

    VkFormat VkFormatFromVertexAttribFormat(VertexAttribFormat& attribFormat);

private: // Internal Members
    // - References
    DeviceContextVk* m_Device = nullptr;
    SwapChainVk* m_SwapChain = nullptr;
    ShaderVk* m_Shader = nullptr;
    RenderContextVk* m_RenderContext = nullptr;
    IShaderResourceBinding* m_StaticBinding = nullptr;
    IShaderResourceBinding* m_Binding = nullptr;

    // - Data
    std::string m_Name;

private: // Vulkan Members
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkPipeline m_Pipeline = nullptr;
    VkDescriptorPool m_StaticDescriptorPool = nullptr;
    VkDescriptorPool m_DescriptorPool = nullptr;
    
    struct ResourceLocation {
        Uint32 set; Uint32 binding;
        bool operator<(const ResourceLocation& t) const
        {
            return (this->set < t.set || (this->set == t.set && this->binding < t.binding));
        }
    };


    std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts{};
    std::map<Uint32, std::vector<VkDescriptorSet>> m_DescriptorSetsPerFrame{};
    std::vector<VkSampler> m_Samplers{};

    bool m_DynamicOffsetsDirty = true;
    Uint32 m_DynamicOffsetCount = 0;
    std::vector<Uint32> m_DynamicOffsets{};
    std::map<ResourceLocation, BufferVk*> m_DynamicOffsetBuffers{};
    std::map<ResourceLocation, ShaderResourceVariableDefinition> m_ShaderVariableDefinitions;
    std::map<ResourceLocation, ShaderResourceVariableDesc> m_ShaderResourceVariables;
};

}
