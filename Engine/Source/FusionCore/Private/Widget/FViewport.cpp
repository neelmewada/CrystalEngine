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

        RPISystem::Get().QueueDestroy(textureSrg);
        textureSrg = nullptr;
    }

    void FViewport::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (IsDefaultInstance())
            return;

        FusionApplication::Get()->DeregisterViewport(this);

        Release();
    }

    void FViewport::Construct()
    {
        Super::Construct();

        auto app = FusionApplication::Get();

        RHI::ShaderResourceGroupLayout textureSrgLayout = app->GetFusionShader2()->GetDefaultVariant()->GetSrgLayout(RHI::SRGType::PerDraw);
        if (textureSrg == nullptr)
        {
            textureSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(textureSrgLayout);
        }

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
        textureDescriptor.texture.dimension = Dimension::Dim2DArray;

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

        for (int i = 0; i < frames.GetSize(); ++i)
        {
            textureSrg->Bind(i, "_Texture", frames[i]->GetRhiTexture());
            textureSrg->Bind(i, "_TextureSampler", frames[i]->GetSamplerState());
        }

        textureSrg->FlushBindings();

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
            painter->DrawViewport(Rect::FromSize(computedPosition, computedSize), this);
        }
    }

}

