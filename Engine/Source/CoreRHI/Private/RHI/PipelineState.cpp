#include "CoreRHI.h"

namespace CE::RHI
{

    RHI::ShaderStage ShaderStageDescriptor::GetShaderStage() const
    {
        if (shaderModule == nullptr)
            return RHI::ShaderStage::None;
        return shaderModule->GetShaderStage();
    }

    SIZE_T ShaderStageDescriptor::GetHash() const
    {
        if (shaderModule == nullptr)
            return 0;

        SIZE_T hash = shaderModule->GetHash();
        CombineHash(hash, entryPoint.GetHashValue());
        return hash;
    }

    SIZE_T GraphicsPipelineDescriptor::GetHash() const
    {
        SIZE_T hash = PipelineDescriptor::GetHash();

        for (const auto& vertexAttrib : vertexAttributes)
        {
            CombineHash(hash, vertexAttrib);
        }

        for (const auto& inputSlot : vertexInputSlots)
        {
            CombineHash(hash, inputSlot);
        }

        CombineHash(hash, rasterState);
        CombineHash(hash, blendState);
        CombineHash(hash, depthStencilState);
        CombineHash(hash, multisampleState);

        return hash;
    }

} // namespace CE::RHI
