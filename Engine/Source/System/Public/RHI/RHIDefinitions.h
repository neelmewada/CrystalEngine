#pragma once

namespace CE
{

    enum class RHIGraphicsBackend
    {
        None,
        Vulkan,
        Metal
    };

    enum class RHIResourceType
    {
        None,
        Buffer,
        RenderTarget
    };

    enum class RHIDeviceObjectType
    {
        None,
        Buffer,
        Texture
    };

    enum class RHIRenderTargetType
    {
        Window,
        Offscreen
    };
    
} // namespace CE
