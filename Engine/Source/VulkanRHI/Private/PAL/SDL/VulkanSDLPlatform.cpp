
#include "PAL/Common/VulkanPlatform.h"

#include "imgui.h"
#include "PAL/SDL/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"


namespace CE
{
	Vec2i VulkanSDLPlatform::GetScreenSizeForWindow(void* sdlWindowHandle)
	{
		SDL_Window* sdlWindow = (SDL_Window*)sdlWindowHandle;

		SDL_DisplayMode mode{};
		int displayIndex = 0;
		if (sdlWindow != nullptr)
			displayIndex = SDL_GetWindowDisplayIndex(sdlWindow);
		if (SDL_GetCurrentDisplayMode(displayIndex, &mode) != 0)
		{
			CE_LOG(Error, All, "Failed to get screen size for window. Error: {}", SDL_GetError());
			return Vec2i();
		}
		return Vec2i(mode.w, mode.h);
	}

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

