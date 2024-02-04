#pragma once

namespace CE::RHI
{
	class Viewport;
	class ShaderResourceGroup;
	class Texture;
	class Buffer;
	class CommandList;
	class IDeviceObject;

	enum class CommandListType
	{
		Direct = 0,
		Indirect
	};

	struct ResourceBarrierDescriptor
	{
		IDeviceObject* resource;
		ResourceState fromState;
		ResourceState toState;
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

		virtual void ResourceBarrier(u32 count, ResourceBarrierDescriptor* barriers) = 0;

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

		virtual void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) = 0;

		virtual void CopyTextureRegion(const BufferToTextureCopy& region) = 0;

	protected:

		RHI::CommandListType commandListType = RHI::CommandListType::Direct;

	};

} // namespace CE::RHI
