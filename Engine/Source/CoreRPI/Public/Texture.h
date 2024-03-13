#pragma once

namespace CE::RPI
{
    struct TextureDescriptor
    {
        RHI::TextureDescriptor texture{};
        RHI::SamplerDescriptor samplerDesc{};
        BinaryBlob* source = nullptr;
    };
    
    class CORERPI_API Texture
    {
    public:
        Texture(const TextureDescriptor& desc);
        Texture(RHI::Texture* texture, const RHI::SamplerDescriptor& samplerDesc = {});
        Texture(RHI::TextureView* textureView, const RHI::SamplerDescriptor& samplerDesc = {});
        Texture(const CMImage& sourceImage, const RHI::SamplerDescriptor& samplerDesc = {});

        virtual ~Texture();

        inline RHI::Texture* GetRhiTexture() const { return texture; }
        inline RHI::Sampler* GetSamplerState() const { return samplerState; }
        inline RHI::TextureView* GetRhiTextureView() const { return textureView; }

        virtual void UploadData(u8* src, u64 dataSize);

    protected:

        RHI::Texture* texture = nullptr;
        RHI::TextureView* textureView = nullptr;
        RHI::Sampler* samplerState = nullptr;
    };

} // namespace CE::RPI
