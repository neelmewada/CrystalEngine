#pragma once

namespace CE::RPI
{
    struct GraphicsPipelineVariant
    {
        RHI::MultisampleState sampleState{};
        Array<RHI::Format> colorFormats{};
        RHI::Format depthStencilFormat = RHI::Format::Undefined;

        inline SIZE_T GetHash() const
        {
            SIZE_T hash = sampleState.GetHash();
            for (int i = 0; i < colorFormats.GetSize(); i++)
            {
                CombineHash(hash, colorFormats[i]);
            }
            CombineHash(hash, depthStencilFormat);
            return hash;
        }

        inline bool operator==(const GraphicsPipelineVariant& rhs) const
        {
            return GetHash() == rhs.GetHash();
        }
    };

    class CORERPI_API GraphicsPipelineCollection
    {
    public:

        GraphicsPipelineCollection(const RHI::GraphicsPipelineDescriptor& desc);

        ~GraphicsPipelineCollection();

        RHI::PipelineState* GetPipeline(const GraphicsPipelineVariant& variant);

        inline RHI::PipelineState* GetPipeline() const { return defaultPipeline; }

        inline const GraphicsPipelineVariant& GetDefaultVariant() const { return defaultVariant; }

    private:

        RHI::GraphicsPipelineDescriptor desc{};

        GraphicsPipelineVariant defaultVariant{};
        RHI::PipelineState* defaultPipeline = nullptr;

        SharedMutex mutex{};
        HashMap<GraphicsPipelineVariant, RHI::PipelineState*> pipelineCollection{};
        Array<RHI::RenderTarget*> renderTargets{};
    };
    
} // namespace CE::RPI
