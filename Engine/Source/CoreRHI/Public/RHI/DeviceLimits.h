#pragma once

namespace CE::RHI
{

    class DeviceLimits : RHI::RHIResource
    {
    protected:
        DeviceLimits() : RHI::RHIResource(ResourceType::DeviceLimits) {}
        virtual ~DeviceLimits() = default;

    public:

        inline bool IsFormatSupported(RHI::Format format, RHI::TextureBindFlags bindFlags)
        {
            return (imageFormatSupport[format].bindFlags & bindFlags) == bindFlags;
        }

        inline bool IsFormatSupported(RHI::Format format, RHI::TextureBindFlags bindFlags, RHI::FilterMode filterMode)
        {
            return IsFormatSupported(format, bindFlags) && (imageFormatSupport[format].filterMask & (1 << (u32)filterMode)) != 0;
        }

        inline u64 GetMaxConstantBufferRange() const { return maxConstantBufferRange; }
        inline u64 GetMaxStructuredBufferRange() const { return maxStructuredBufferRange; }

    protected:

        struct FormatEntry
        {
            RHI::TextureBindFlags bindFlags{};
            RHI::FilterModeMask filterMask{};
        };

        HashMap<RHI::Format, FormatEntry> imageFormatSupport{};

        u32 maxConstantBufferRange = 0;
        u32 maxStructuredBufferRange = 0;
    };
    
} // namespace CE::RHI
