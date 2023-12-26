#pragma once

#include "RHIDefinitions.h"

namespace CE::RHI
{
    
    class CORERHI_API RHIResource : IntrusiveBase
    {
    protected:
		RHIResource(ResourceType resourceType) : resourceType(resourceType)
        {}
    public:
        virtual ~RHIResource() = default;

        ResourceType GetResourceType() const 
        {
            return resourceType;
        }
        
    private:
        ResourceType resourceType = ResourceType::None;
    };

    class IDeviceObject
    {
    protected:
        IDeviceObject(DeviceObjectType type) : deviceObjectType(type)
        {}
    public:
        virtual ~IDeviceObject() {}

        virtual DeviceObjectType GetDeviceObjectType() const
        {
            return deviceObjectType;
        }
        
    private:
        DeviceObjectType deviceObjectType = DeviceObjectType::None;
    };
    
} // namespace CE
