#pragma once

namespace CE::RHI
{
	class IPipelineState;
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

		virtual CommandListType GetCommandListType() = 0;

		virtual void WaitForExecution() = 0;

		// - Command List API -

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void BindPipeline(RHI::IPipelineState* pipeline) = 0;

		virtual void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& shaderResourceGroups) = 0;

		virtual void SetShaderResourceGroup(RHI::ShaderResourceGroup* shaderResourceGroup)
		{
			SetShaderResourceGroups({ shaderResourceGroup });
		}

		virtual void CommitShaderResourceGroups() = 0;

		virtual void SetRootConstants(u8 size, const u8* data) = 0;

	};

	class CORERHI_API GraphicsCommandList : public CommandList
	{
	protected:
		GraphicsCommandList() : CommandList()
		{}

	public:
		virtual ~GraphicsCommandList() = default;

		// - Public API -

		virtual CommandListType GetCommandListType() override final { return CommandListType::Graphics; }

		// - Graphics Command List API -

		virtual void SetScissorRects(u32 scissorCount, const RHI::ScissorState* scissors) = 0;
		virtual void SetViewportRects(u32 viewportCount, const RHI::ViewportState* viewports) = 0;

		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers) = 0;
		virtual void BindVertexBuffers(u32 firstBinding, const Array<RHI::Buffer*>& buffers, const Array<SIZE_T>& bufferOffsets) = 0;

		virtual void BindIndexBuffer(RHI::Buffer* buffer, bool use32BitIndex, SIZE_T offset) = 0;

		virtual void DrawIndexed(u32 indexCount, u32 instanceCount, u32 firstIndex, s32 vertexOffset, u32 firstInstance) = 0;

		// - ImGui Setup (Deprecated) -

		virtual bool InitImGui(FontPreloadConfig* preloadFonts, Array<void*>& outFontHandles) = 0;

		virtual void ShutdownImGui() = 0;

		virtual void ImGuiNewFrame() = 0;

		virtual void ImGuiRender() = 0;

		virtual void ImGuiPlatformUpdate() = 0;

	};
    
} // namespace CE::RHI
