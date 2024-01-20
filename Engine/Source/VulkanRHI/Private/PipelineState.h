#pragma once

#include "VulkanRHIPrivate.h"

namespace CE::Vulkan
{
	class PipelineState;
	class GraphicsPipeline;

    class PipelineState : public RHI::PipelineState
    {
    public:
        PipelineState(VulkanDevice* device, const RHI::GraphicsPipelineDescriptor& graphicsDesc);
        virtual ~PipelineState();

    protected:

        VulkanDevice* device = nullptr;

		Pipeline* pipeline = nullptr;

		friend class GraphicsCommandList;
		friend class PipelineLayout;
		friend class GraphicsPipelineState;
    };


	/*class GraphicsPipelineState : public PipelineState, public RHI::GraphicsPipelineState
	{
	public:
		GraphicsPipelineState(VulkanDevice* device, RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc);
		virtual ~GraphicsPipelineState();

		bool IsGraphicsPipelineState() override final
		{
			return true;
		}

		bool IsComputePipelineState() override final
		{
			return false;
		}

		void* GetNativeHandle() override { return pipeline; }

		RHI::IPipelineLayout* GetPipelineLayout() override
		{
			return pipelineLayout;
		}

	protected:

		void Create(RenderTarget* renderTarget, const RHI::GraphicsPipelineDescriptor& desc);
		
		void Destroy();

	private:

		friend class GraphicsCommandList;
		friend class PipelineLayout;
	};*/
    
} // namespace CE::Editor
