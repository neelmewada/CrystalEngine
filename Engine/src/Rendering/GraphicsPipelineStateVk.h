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

public: // Public API

private: // Internal API
    void CreateGraphicsPipeline(const GraphicsPipelineStateCreateInfo& createInfo);
    VkFormat VkFormatFromVertexAttribFormat(VertexAttribFormat& attribFormat);

private: // Internal Members
    // - References
    DeviceContextVk* m_pDevice = nullptr;
    SwapChainVk* m_pSwapChain = nullptr;
    ShaderVk* m_pShader = nullptr;

private: // Vulkan Members
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkPipeline m_Pipeline = nullptr;
};

}
