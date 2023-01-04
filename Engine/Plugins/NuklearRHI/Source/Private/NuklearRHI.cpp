
#include "NuklearRHI.h"
#include "NuklearVulkanRHI.h"

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

} // namespace CE
