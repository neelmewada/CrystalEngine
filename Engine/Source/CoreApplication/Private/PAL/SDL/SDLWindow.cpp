
#include "CoreApplication.h"

#include <SDL.h>
#if PAL_TRAIT_VULKAN_SUPPORTED
#include <SDL_vulkan.h>
#endif

namespace CE
{

	SDLPlatformWindow::SDLPlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen)
	{
		u32 flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
#if PAL_TRAIT_VULKAN_SUPPORTED
		flags |= SDL_WINDOW_VULKAN;
#endif
		if (maximised)
			flags |= SDL_WINDOW_MAXIMIZED;
		if (fullscreen)
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		handle = SDL_CreateWindow(title.GetCString(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	}

	void SDLPlatformWindow::GetWindowSize(u32* outWidth, u32* outHeight)
	{
		int w = 0, h = 0;
		SDL_GetWindowSize(handle, &w, &h);
		*outWidth = (u32)w;
		*outHeight = (u32)h;
	}

	void SDLPlatformWindow::GetDrawableWindowSize(u32* outWidth, u32* outHeight)
	{
		int w = 0, h = 0;
#if PAL_TRAIT_VULKAN_SUPPORTED
		SDL_Vulkan_GetDrawableSize(handle, &w, &h);
#else
		GetWindowSize(outWidth, outHeight);
		return;
#endif
		*outWidth = (u32)w;
		*outHeight = (u32)h;
	}

	VkSurfaceKHR SDLPlatformWindow::CreateVulkanSurface(VkInstance instance)
	{
		VkSurfaceKHR surface = nullptr;
		SDL_Vulkan_CreateSurface(handle, instance, &surface);
		return surface;
	}

	u32 SDLPlatformWindow::GetWindowId()
	{
		return SDL_GetWindowID(handle);
	}

	SDLPlatformWindow::~SDLPlatformWindow()
	{
		if (handle != nullptr)
			SDL_DestroyWindow(handle);
		handle = nullptr;
	}

	void* SDLPlatformWindow::GetUnderlyingHandle()
	{
		return handle;
	}

	void SDLPlatformWindow::Show()
	{
		SDL_ShowWindow(handle);
	}

	void SDLPlatformWindow::Hide()
	{
		SDL_HideWindow(handle);
	}

} // namespace CE

