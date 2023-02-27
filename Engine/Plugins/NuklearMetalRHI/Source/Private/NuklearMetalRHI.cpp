
#include "CoreMinimal.h"

#include "NuklearMetalRHI.h"
#include "MetalDevice.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "Metal/Metal.hpp"
#include "QuartzCore/QuartzCore.hpp"

CE_IMPLEMENT_PLUGIN(NuklearMetalRHI, CE::NuklearMetalRHIModule)

namespace CE
{
    void NuklearMetalRHIModule::StartupModule()
	{
        
	}

	void NuklearMetalRHIModule::ShutdownModule()
	{

	}

	void NuklearMetalRHIModule::RegisterTypes()
	{

	}



    void NuklearMetalRHI::Initialize()
    {
        
    }

    void NuklearMetalRHI::PostInitialize()
    {
        device = new MetalDevice(this);
        device->Initialize();
    }

    void NuklearMetalRHI::PreShutdown()
    {
        device->PreShutdown();
    }

    void NuklearMetalRHI::Shutdown()
    {
        device->Shutdown();
        delete device;
    }
    
    void NuklearMetalRHI::GetNativeHandle(void** outInstance)
    {

    }
    
    RHIGraphicsBackend NuklearMetalRHI::GetGraphicsBackend()
    {
        return RHIGraphicsBackend::Metal;
    }
}

