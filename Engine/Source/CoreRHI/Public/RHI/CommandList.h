#pragma once

namespace CE::RHI
{
	class Viewport;
	class ShaderResourceGroup;

	class CORERHI_API CommandList : public RHIResource
	{
	protected:
		CommandList() : RHIResource(ResourceType::CommandList)
		{}
	public:
		virtual ~CommandList() = default;

		// - Public API -


		// - Command List API -

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs) = 0;

		inline void SetShaderResourceGroup(RHI::ShaderResourceGroup* srg)
		{
			if (srg != nullptr)
			{
				SetShaderResourceGroups({ srg });
			}
		}

		virtual void CommitShaderResources() = 0;

		virtual void BindPipelineState(RHI::PipelineState* pipelineState) = 0;

		virtual void BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews) = 0;

		virtual void BindIndexBuffer(const RHI::IndexBufferView& bufferView) = 0;

		virtual void DrawIndexed(const DrawIndexedArguments& args) = 0;

	};

} // namespace CE::RHI
