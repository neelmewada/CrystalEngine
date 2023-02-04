
#include "NuklearRHI.h"

#if PAL_TRAIT_VULKAN_SUPPORTED
#include "NuklearVulkanRHI.h"
#endif

CE_IMPLEMENT_PLUGIN(NuklearRHI, CE::NuklearRHIModule)

namespace CE
{
    NUKLEARRHI_API DynamicRHI* GDynamicRHI = nullptr;

    void NuklearRHIModule::StartupModule()
    {
        Initialize();
    }

    void NuklearRHIModule::ShutdownModule()
    {
        PreShutdown();
    }

    void NuklearRHIModule::RegisterTypes()
    {

    }

    void NuklearRHIModule::Initialize()
    {
#if PAL_TRAIT_VULKAN_SUPPORTED
        GDynamicRHI = new CE::NuklearVulkanRHI();
#else
#   error No valid Graphics API Supported by target platform
#endif

        RHIBus::BusConnect(GDynamicRHI);

        GDynamicRHI->Initialize();
    }

    void NuklearRHIModule::PreShutdown()
    {
        GDynamicRHI->PreShutdown();

        RHIBus::BusDisconnect(GDynamicRHI);

        GDynamicRHI->Shutdown();

        delete GDynamicRHI;
        GDynamicRHI = nullptr;
    }

} // namespace CE
