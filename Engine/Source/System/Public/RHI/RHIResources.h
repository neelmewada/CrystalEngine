#pragma once

#include "RHIDefinitions.h"

namespace CE
{
    
    class SYSTEM_API RHIResource
    {
    protected:
        RHIResource(RHIResourceType resourceType) : resourceType(resourceType)
        {}
    public:
        virtual ~RHIResource() = default;
        
    private:
        RHIResourceType resourceType = RHIResourceType::None;
    };

    class IRHIDeviceObject
    {
    protected:
        IRHIDeviceObject(RHIDeviceObjectType type) : deviceObjectType(type)
        {}
    public:
        virtual ~IRHIDeviceObject() {}

        virtual RHIDeviceObjectType GetDeviceObjectType() const
        {
            return deviceObjectType;
        }
        
    private:
        RHIDeviceObjectType deviceObjectType = RHIDeviceObjectType::None;
    };

    class SYSTEM_API RHIBuffer : public RHIResource, public IRHIDeviceObject
    {
    protected:
        RHIBuffer() : RHIResource(RHIResourceType::Buffer), IRHIDeviceObject(RHIDeviceObjectType::Buffer)
        {}

    public:
        virtual ~RHIBuffer() = default;
        
        virtual RHIBufferBindFlags GetBindFlags() = 0;

        virtual void* GetHandle() = 0;

        virtual void UploadData(const RHIBufferData& data) = 0;

    };
    
    class SYSTEM_API RHITexture : public RHIResource, public IRHIDeviceObject
    {
    public:
        RHITexture() : RHIResource(RHIResourceType::Texture), IRHIDeviceObject(RHIDeviceObjectType::Texture)
        {}
        
    public:
        virtual ~RHITexture() = default;

        virtual void* GetHandle() = 0;

        virtual u32 GetWidth() = 0;
        virtual u32 GetHeight() = 0;
        virtual u32 GetDepth() = 0;
        virtual u32 GetBytesPerChannel() = 0;
        virtual u32 GetNumberOfChannels() = 0;

        virtual void UploadData(const void* pixels) = 0;
    };
    
} // namespace CE
