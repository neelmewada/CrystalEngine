#include "FusionCore.h"

namespace CE
{

    FViewport::FViewport()
    {
        m_ImageFormat = Format::B8G8R8A8_UNORM;
        m_SampleCount = 1;
    }

    FViewport::~FViewport()
    {
        for (int i = 0; i < frames.GetSize(); ++i)
        {
            if (frames[i] != nullptr)
            {
                delete frames[i]; frames[i] = nullptr;
            }
        }
    }

    void FViewport::Construct()
    {
        Super::Construct();


    }

    void FViewport::OnFusionPropertyModified(const CE::Name& propertyName)
    {
	    Super::OnFusionPropertyModified(propertyName);

        static const CE::Name imageFormat = "ImageFormat";
        static const CE::Name sampleCount = "SampleCount";

        if (propertyName == imageFormat || propertyName == sampleCount)
        {
            RecreateFrameBuffer();
        }
    }

    void FViewport::RecreateFrameBuffer()
    {
        if (currentSize.x <= 0 || currentSize.y <= 0)
            return;

        for (int i = 0; i < frames.GetSize(); ++i)
        {
	        if (frames[i] != nullptr)
	        {
                delete frames[i]; frames[i] = nullptr;
	        }
        }

        RPI::TextureDescriptor textureDescriptor{};

        BinaryBlob blob{};
        blob.Reserve((u64)currentSize.x * currentSize.y * 4);
        u8* ptr = blob.GetDataPtr();
        for (int y = 0; y < currentSize.y; ++y)
        {
            for (int x = 0; x < currentSize.x; ++x)
            {
                int i = y * currentSize.x * 4 + x * 4;
                u8 bgra[] = { 0, 0, 0, 255 };
                if (x < currentSize.x / 3.0f)
                    bgra[2] = 255;
                else if (x < currentSize.x * 2.0f / 3.0f)
                    bgra[1] = 255;
                else
                    bgra[0] = 255;

                *(ptr + i + 0) = bgra[0];
                *(ptr + i + 1) = bgra[1];
                *(ptr + i + 2) = bgra[2];
                *(ptr + i + 3) = bgra[3];
            }
        }

        textureDescriptor.texture.name = GetName().GetString() + " FrameBuffer";
        textureDescriptor.texture.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
        textureDescriptor.texture.arrayLayers = 1;
        textureDescriptor.texture.mipLevels = 1;
        textureDescriptor.texture.sampleCount = m_SampleCount;
        textureDescriptor.texture.format = m_ImageFormat;
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

        textureDescriptor.source = &blob;

        for (int i = 0; i < frames.GetSize(); ++i)
        {
            frames[i] = new RPI::Texture(textureDescriptor);
        }

        blob.Free();
        auto app = FusionApplication::Get();

        if (imageIndex < 0)
        {
            for (int i = 0; i < frames.GetSize(); ++i)
            {
	            const int idx = app->RegisterImage(String::Format("FViewport_{}_{}", GetUuid(), i), frames[i]->GetRhiTexture());
                if (i == 0)
                {
                    imageIndex = idx;
                }
            }
        }
        else
        {
            for (int i = 0; i < frames.GetSize(); ++i)
            {
                app->ReplaceImage(imageIndex + i, frames[i]->GetRhiTexture());
            }
        }

        m_OnFrameBufferRecreated();
    }

    void FViewport::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        if ((int)computedSize.x != currentSize.x || (int)computedSize.y != currentSize.y)
        {
            currentSize = computedSize.ToVec2i();

            RecreateFrameBuffer();
        }

        // TODO: Draw framebuffer

        //painter->DrawFrameBuffer();
    }

}

