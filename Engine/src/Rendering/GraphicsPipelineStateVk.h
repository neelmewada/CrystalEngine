#pragma once

#include "Rendering/IGraphicsPipelineState.h"

#include "Rendering/DeviceContextVk.h"

namespace Vox
{

class GraphicsPipelineStateVk : public IGraphicsPipelineState
{
public:
    GraphicsPipelineStateVk(const GraphicsPipelineStateCreateInfo& createInfo);
    ~GraphicsPipelineStateVk();
};

}
