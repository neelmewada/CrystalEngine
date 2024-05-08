#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CViewport::CViewport()
    {
	    
    }

    CViewport::~CViewport()
    {
        for (int i = 0; i < frames.GetSize(); ++i)
        {
            if (frames[i] != nullptr)
            {
                delete frames[i]; frames[i] = nullptr;
            }
        }
    }

    void CViewport::RecreateFrameBuffer()
    {
        if (currentSize.x <= 0 || currentSize.y <= 0)
            return;

        RPI::TextureDescriptor textureDescriptor{};

        textureDescriptor.texture.name = GetName().GetString() + " FrameBuffer";
        textureDescriptor.texture.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
        textureDescriptor.texture.arrayLayers = 1;
        textureDescriptor.texture.mipLevels = 1;
        textureDescriptor.texture.sampleCount = sampleCount;
        textureDescriptor.texture.format = format;
        textureDescriptor.texture.defaultHeapType = MemoryHeapType::Default;
        textureDescriptor.texture.width = currentSize.x;
        textureDescriptor.texture.height = currentSize.y;
        textureDescriptor.texture.dimension = Dimension::Dim2D;

        textureDescriptor.samplerDesc.addressModeU =
            textureDescriptor.samplerDesc.addressModeV =
            textureDescriptor.samplerDesc.addressModeW =
            SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.borderColor = SamplerBorderColor::FloatOpaqueBlack;
        textureDescriptor.samplerDesc.enableAnisotropy = false;
        textureDescriptor.samplerDesc.samplerFilterMode = FilterMode::Linear;

        for (int i = 0; i < frames.GetSize(); ++i)
        {
	        if (frames[i] != nullptr)
	        {
                delete frames[i]; frames[i] = nullptr;
	        }

            frames[i] = new RPI::Texture(textureDescriptor);
        }
    }

    void CViewport::OnPaint(CPaintEvent* paintEvent)
    {
        CPainter* painter = paintEvent->painter;

        Vec2i size = GetComputedLayoutSize().ToVec2i();

        if (size != currentSize)
        {
            currentSize = size;

            RecreateFrameBuffer();
        }

	    Super::OnPaint(paintEvent);
    }

} // namespace CE::Widgets
