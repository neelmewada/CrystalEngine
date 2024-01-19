
#include "CoreApplication.h"

#include <SDL.h>
#if PAL_TRAIT_VULKAN_SUPPORTED
#include <SDL_vulkan.h>
#endif

#include <SDL_syswm.h>

namespace CE
{
	SDLPlatformWindow::~SDLPlatformWindow()
	{
		if (handle != nullptr)
			SDL_DestroyWindow(handle);
		handle = nullptr;
	}

	SDLPlatformWindow::SDLPlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable)
	{
		u32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
		if (resizable)
			flags |= SDL_WINDOW_RESIZABLE;
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
#if PAL_TRAIT_VULKAN_SUPPORTED
		int w = 0, h = 0;
		SDL_Vulkan_GetDrawableSize(handle, &w, &h);
		*outWidth = (u32)w;
		*outHeight = (u32)h;
#else
		GetWindowSize(outWidth, outHeight);
		return;
#endif
	}

	void SDLPlatformWindow::SetResizable(bool resizable)
	{
		SDL_SetWindowResizable(handle, resizable ? SDL_TRUE : SDL_FALSE);
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

	void* SDLPlatformWindow::GetUnderlyingHandle()
	{
		return handle;
	}

	WindowHandle SDLPlatformWindow::GetOSNativeHandle()
	{
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(handle, &wmInfo);
#if PLATFORM_WINDOWS
		return wmInfo.info.win.window;
#elif PLATFORM_MAC
        return wmInfo.info.cocoa.window;
#else
#   error Platform window handle not specified
#endif
	}

	void SDLPlatformWindow::Show()
	{
		SDL_ShowWindow(handle);
	}

	void SDLPlatformWindow::Hide()
	{
		SDL_HideWindow(handle);
	}

	bool SDLPlatformWindow::IsFocussed()
	{
		Uint32 flags = SDL_GetWindowFlags(handle);
		return (flags & (SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS)) != 0;
	}

} // namespace CE

