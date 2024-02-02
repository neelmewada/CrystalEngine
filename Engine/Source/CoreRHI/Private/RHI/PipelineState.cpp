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

    SIZE_T VertexAttributeDescriptor::GetHash() const
    {
        if (dataType == VertexAttributeDataType::Undefined)
            return 0;

        SIZE_T hash = CE::GetHash(dataType);
        CombineHash(hash, location);
        CombineHash(hash, offset);
        CombineHash(hash, inputSlot);
        return hash;
    }

    SIZE_T StencilOpState::GetHash() const
    {
        SIZE_T hash = CE::GetHash(failOp);
        CombineHash(hash, depthFailOp);
        CombineHash(hash, passOp);
        CombineHash(hash, compareOp);
        return hash;
    }

    SIZE_T PipelineDescriptor::GetHash() const
    {
        SIZE_T hash = 0;
        for (const auto& shaderStage : shaderStages)
        {
            if (hash == 0)
                hash = shaderStage.GetHash();
            else
                CombineHash(hash, shaderStage);
        }

        return hash;
    }

    SIZE_T StencilState::GetHash() const
    {
        SIZE_T hash = CE::GetHash(enable);
        if (!enable)
            return hash;
        CombineHash(hash, readMask);
        CombineHash(hash, writeMask);
        CombineHash(hash, frontFace);
        CombineHash(hash, backFace);
        return hash;
    }

    SIZE_T DepthStencilState::GetHash() const
    {
        SIZE_T hash = depthState.GetHash();
        CombineHash(hash, stencilState);
        return hash;
    }

    SIZE_T RasterState::GetHash() const
    {
        SIZE_T hash = CE::GetHash(cullMode);
        CombineHash(hash, fillMode);
        CombineHash(hash, multisampleEnable);
        CombineHash(hash, depthClipEnable);
        return hash;
    }

} // namespace CE::RHI
