
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

    }

    void NuklearRHIModule::ShutdownModule()
    {

    }

    void NuklearRHIModule::Initialize()
    {
#if PAL_TRAIT_VULKAN_SUPPORTED
        GDynamicRHI = new CE::NuklearVulkanRHI();
#endif
    }

    void NuklearRHIModule::PreShutdown()
    {
        delete GDynamicRHI;
        GDynamicRHI = nullptr;
    }

} // namespace CE
