#pragma once

namespace CE
{
    enum GraphicsAPI
    {
        GFX_NONE,
        GFX_VULKAN
    };

    enum class RHIResourceType
    {
        None,
        Buffer,
    };

    enum class RHIDeviceObjectType
    {
        None,
        Buffer,
        Texture
    };

    class SYSTEM_API RHIResource
    {
    public:
        RHIResource(RHIResourceType resourceType) : resourceType(resourceType)
        {}
        
        virtual ~RHIResource() = default;
        
    private:
        RHIResourceType resourceType = RHIResourceType::None;
    };

    class IRHIDeviceObject
    {
    public:
        IRHIDeviceObject(RHIDeviceObjectType type) : deviceObjectType(type)
        {}
        
        virtual ~IRHIDeviceObject() {}
        
    private:
        RHIDeviceObjectType deviceObjectType = RHIDeviceObjectType::None;
    };

    class SYSTEM_API RHIBuffer : public RHIResource, public IRHIDeviceObject
    {
    public:
        RHIBuffer() : RHIResource(RHIResourceType::Buffer), IRHIDeviceObject(RHIDeviceObjectType::Buffer)
        {}
        
        virtual ~RHIBuffer() = default;
        
    };
    
} // namespace CE
