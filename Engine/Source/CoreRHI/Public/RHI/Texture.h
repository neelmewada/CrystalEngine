#pragma once

namespace CE::RHI
{

	struct TextureDescriptor
	{
		Name name{};
		u32 width = 0, height = 0, depth = 1;
		Dimension dimension = Dimension::Dim2D;
		Format format{};
		u32 mipLevels = 1;
		u32 sampleCount = 1;
		u32 arrayLayers = 1;
		TextureBindFlags bindFlags = TextureBindFlags::ShaderRead;
		MemoryHeapType defaultHeapType = MemoryHeapType::Default;
	};

    using ImageDescriptor = TextureDescriptor;

	class CORERHI_API Texture : public RHIResource, public IDeviceObject
	{
	protected:
		Texture() : RHIResource(ResourceType::Texture), IDeviceObject(DeviceObjectType::Texture)
		{}

	public:
		virtual ~Texture() = default;

		virtual void* GetHandle() = 0;
        
        inline u32 GetWidth() const { return width; }
        
		inline u32 GetHeight() const { return height; }
        
		inline u32 GetDepth() const { return depth; }

		inline u32 GetArrayLayerCount() const { return arrayLayers; }

		inline u32 GetMipLevelCount() const { return mipLevels; }

		inline const Name& GetDebugName() const { return name; }

		inline RHI::Format GetFormat() const { return format; }

		inline Dimension GetDimension() const { return dimension; }

		inline u32 GetSampleCount() const { return sampleCount; }

		inline TextureBindFlags GetBindFlags() const { return bindFlags; }
        
		virtual u32 GetBytesPerChannel() = 0;
        
		virtual u32 GetNumberOfChannels() = 0;

	protected:

		Name name{};

		u32 width = 0, height = 0, depth = 0;
		Dimension dimension = Dimension::Dim2D;
		Format format{};
		u32 mipLevels = 1;
		u32 sampleCount = 1;
		u32 arrayLayers = 1;
		TextureBindFlags bindFlags = TextureBindFlags::None;
		
	};
    
} // namespace CE::RHI
