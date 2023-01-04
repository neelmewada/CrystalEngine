
#include "NuklearRHI.h"

#if VULKAN_SUPPORTED
#include "NuklearVulkanRHI.h"
#endif

CE_IMPLEMENT_PLUGIN(NuklearRHI, CE::NuklearRHIModule)

namespace CE
{
    NUKLEARRHI_API DynamicRHI* GDynamicRHI = nullptr;

    void NuklearRHIModule::StartupModule()
    {
#if VULKAN_SUPPORTED

#endif
    }

    void NuklearRHIModule::ShutdownModule()
    {

    }

} // namespace CE
