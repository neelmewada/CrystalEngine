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
        
    }

    void FViewport::Release()
    {
        for (int i = 0; i < frames.GetSize(); ++i)
        {
            if (frames[i] != nullptr)
            {
                delete frames[i]; frames[i] = nullptr;
            }
        }
    }

    void FViewport::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (IsDefaultInstance())
            return;

        FusionApplication::Get()->DeregisterViewport(this);

        Release();
    }

    void FViewport::Construct()
    {
        Super::Construct();

        FusionApplication::Get()->RegisterViewport(this);
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

    void FViewport::OnPostComputeLayout()
    {
	    Super::OnPostComputeLayout();

        if ((int)computedSize.x != currentSize.x || (int)computedSize.y != currentSize.y)
        {
            currentSize = computedSize.ToVec2i();

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

        textureDescriptor.texture.name = GetName().GetString() + " FrameBuffer";
        textureDescriptor.texture.bindFlags = TextureBindFlags::Color | TextureBindFlags::ShaderRead;
        textureDescriptor.texture.arrayLayers = 1;
        textureDescriptor.texture.mipLevels = 1;
        textureDescriptor.texture.sampleCount = m_SampleCount;
        textureDescriptor.texture.format = m_ImageFormat;
        textureDescriptor.texture.defaultHeapType = MemoryHeapType::Default;
        textureDescriptor.texture.width = currentSize.x;
        textureDescriptor.texture.height = currentSize.y;
        textureDescriptor.texture.depth = 1;
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
            frames[i] = new RPI::Texture(textureDescriptor);
        }

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

        MarkDirty();

        m_OnFrameBufferRecreated();
    }

    void FViewport::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        painter->SetPen(FPen());
        painter->SetBrush(FBrush());

        if (frames[0] != nullptr)
        {
            painter->DrawFrameBuffer(Rect::FromSize(computedPosition, computedSize), frames);
        }
    }

}

