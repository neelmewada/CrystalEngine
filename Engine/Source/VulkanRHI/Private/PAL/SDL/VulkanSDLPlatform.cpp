
#include "PAL/Common/VulkanPlatform.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"


namespace CE
{

	void VulkanSDLPlatform::InitVulkanForWindow(PlatformWindow* window)
	{
		auto sdlWindow = (SDL_Window*)((SDLPlatformWindow*)window)->GetUnderlyingHandle();
		ImGui_ImplSDL2_InitForVulkan(sdlWindow);
	}

	void VulkanSDLPlatform::ShutdownVulkanForWindow()
	{
		ImGui_ImplSDL2_Shutdown();
	}

} // namespace CE

