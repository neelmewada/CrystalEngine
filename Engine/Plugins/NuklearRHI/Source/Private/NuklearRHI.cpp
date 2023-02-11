
#include "NuklearRHI.h"

#if PAL_TRAIT_VULKAN_SUPPORTED
#include "NuklearVulkanRHI.h"
#endif

CE_IMPLEMENT_PLUGIN(NuklearRHI, CE::NuklearRHIModule)

namespace CE
{

    void NuklearRHIModule::StartupModule()
    {
        Initialize();
    }

    void NuklearRHIModule::ShutdownModule()
    {
        Shutdown();
    }

    void NuklearRHIModule::RegisterTypes()
    {

    }

    void NuklearRHIModule::Initialize()
    {
#if PAL_TRAIT_VULKAN_SUPPORTED
        gDynamicRHI = new CE::NuklearVulkanRHI();
#else
#   error No valid Graphics API Supported by target platform
#endif

    }

    void NuklearRHIModule::Shutdown()
    {
        delete gDynamicRHI;
        gDynamicRHI = nullptr;
    }

} // namespace CE
