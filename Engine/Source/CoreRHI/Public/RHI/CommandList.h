#pragma once

namespace CE::RHI
{
	class Viewport;
	class ShaderResourceGroup;
	class Texture;
	class Buffer;
	class CommandList;
	class IDeviceObject;
	class RenderTarget;
	class RenderTargetBuffer;
	struct AttachmentClearValue;

	enum class CommandListType
	{
		Direct = 0,
		Indirect
	};

	struct SubresourceRange
	{
		u32 baseMipLevel = 0;
		u32 levelCount = 0;
		u32 baseArrayLayer = 0;
		u32 layerCount = 0;

		constexpr static SubresourceRange All()
		{
			SubresourceRange result;
			result.baseMipLevel = result.levelCount = result.baseArrayLayer = result.layerCount = 0;
			return result;
		}

		inline bool IsAll() const
		{
			return baseMipLevel == 0 && levelCount == 0 && baseArrayLayer == 0 && layerCount == 0;
		}
	};

	struct ResourceBarrierDescriptor
	{
		IDeviceObject* resource;
		ResourceState fromState;
		ResourceState toState;
		SubresourceRange subresourceRange = SubresourceRange::All();
	};

	struct BufferToTextureCopy
	{
		RHI::Buffer* srcBuffer = nullptr;
		u64 bufferOffset = 0;

		RHI::Texture* dstTexture = nullptr;
		u16 mipSlice = 0;
		u16 baseArrayLayer = 0;
		u16 layerCount = 1;
	};

	class CORERHI_API CommandList : public RHIResource
	{
	protected:
		CommandList() : RHIResource(ResourceType::CommandList)
		{}
	public:
		virtual ~CommandList() = default;

		// - Public API -

		inline RHI::CommandListType GetCommandListType() const 
		{
			return commandListType;
		}

		// - Command List API -

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void BeginRenderTarget(RenderTarget* renderTarget, RenderTargetBuffer* renderTargetBuffer, AttachmentClearValue* clearValuesPerAttachment) = 0;
		virtual void EndRenderTarget() = 0;

		inline void SetCurrentImageIndex(u32 imageIndex)
		{
			currentImageIndex = imageIndex;
		}

		virtual void ResourceBarrier(u32 count, ResourceBarrierDescriptor* barriers) = 0;

		virtual void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs) = 0;

		inline void SetShaderResourceGroup(RHI::ShaderResourceGroup* srg)
		{
			if (srg != nullptr)
			{
				SetShaderResourceGroups({ srg });
			}
		}

		virtual void ClearShaderResourceGroups() = 0;

		virtual void SetViewports(u32 count, ViewportState* viewports) = 0;
		virtual void SetScissors(u32 count, ScissorState* scissors) = 0;

		virtual void CommitShaderResources() = 0;

		virtual void BindPipelineState(RHI::PipelineState* pipelineState) = 0;

		virtual void BindVertexBuffers(u32 firstInputSlot, u32 count, const RHI::VertexBufferView* bufferViews) = 0;

		virtual void BindIndexBuffer(const RHI::IndexBufferView& bufferView) = 0;

		virtual void DrawIndexed(const DrawIndexedArguments& args) = 0;

		virtual void DrawLinear(const DrawLinearArguments& args) = 0;

		virtual void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) = 0;

		virtual void CopyTextureRegion(const BufferToTextureCopy& region) = 0;

	protected:

		u32 currentImageIndex = 0;

		RHI::CommandListType commandListType = RHI::CommandListType::Direct;

	};

} // namespace CE::RHI
