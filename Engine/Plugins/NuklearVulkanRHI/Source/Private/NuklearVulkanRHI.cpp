
#include "NuklearVulkanRHI.h"
#include "VulkanRHIPrivate.h"

#include "vulkan/vulkan.h"

CE_IMPLEMENT_PLUGIN(NuklearVulkanRHI, CE::NuklearVulkanRHIModule)

namespace CE
{
	void NuklearVulkanRHIModule::StartupModule()
	{

	}

	void NuklearVulkanRHIModule::ShutdownModule()
	{

	}

	void NuklearVulkanRHIModule::RegisterTypes()
	{

	}

	void NuklearVulkanRHI::Initialize()
	{
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion = VK_VERSION_1_0;
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        
        VkInstanceCreateInfo instanceCI{};
        instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCI.pApplicationInfo = &appInfo;
        
        VkResult result = vkCreateInstance(&instanceCI, nullptr, &vkInstance);
        if (result != VK_SUCCESS)
        {
            return;
        }
	}

	void NuklearVulkanRHI::PreShutdown()
	{

	}

	void NuklearVulkanRHI::Shutdown()
	{

	}

	RHIGraphicsBackend NuklearVulkanRHI::GetGraphicsBackend()
	{
		return RHIGraphicsBackend::Vulkan;
	}

} // namespace CE
