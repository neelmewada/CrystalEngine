#pragma once

#include "Definitions.h"

namespace CE::RHI
{
    
    class CORERHI_API Resource : IntrusiveBase
    {
    protected:
        Resource(ResourceType resourceType) : resourceType(resourceType)
        {}
    public:
        virtual ~Resource() = default;

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
