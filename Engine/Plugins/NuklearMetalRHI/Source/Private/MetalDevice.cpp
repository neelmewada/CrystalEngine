
#include "CoreMinimal.h"

#include "MetalDevice.h"

#include "Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"

namespace CE
{

    MetalDevice::MetalDevice(NuklearMetalRHI* metalRhi)
    {
        
    }
    
    MetalDevice::~MetalDevice()
    {
        
    }

    void MetalDevice::Initialize()
    {
        device = MTL::CreateSystemDefaultDevice();
        
        isInitialized = true;
        
        CE_LOG(Info, All, "Metal device initialized");
    }

    void MetalDevice::PreShutdown()
    {
        isInitialized = false;
    }

    void MetalDevice::Shutdown()
    {
        device->release();
        
        CE_LOG(Info, All, "Metal device shutdown");
    }
    
} // namespace CE
