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

        virtual ~Texture();

        inline RHI::Texture* GetRhiTexture() const { return texture; }
        inline RHI::Sampler* GetSamplerState() const { return samplerState; }
        inline RHI::TextureView* GetRhiTextureView() const { return textureView; }

        virtual void UploadData(BinaryBlob* source, int totalMipLevels = 1);

    protected:

        RHI::Texture* texture = nullptr;
        RHI::TextureView* textureView = nullptr;
        RHI::Sampler* samplerState = nullptr;
    };

} // namespace CE::RPI
