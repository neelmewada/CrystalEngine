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
        
    };
    
    
} // namespace CE
