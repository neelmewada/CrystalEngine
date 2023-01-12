
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
