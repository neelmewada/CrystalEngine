
#include "CoreApplication.h"

#include <SDL.h>
#if PAL_TRAIT_VULKAN_SUPPORTED
#include <SDL_vulkan.h>
#endif

#include <SDL_syswm.h>

#define MOUSE_GRAB_PADDING 10
#define WINDOW_DRAG_PADDING 50

namespace CE
{
	SDLPlatformWindow::~SDLPlatformWindow()
	{
		if (handle != nullptr)
			SDL_DestroyWindow(handle);
		handle = nullptr;
	}

	// Credit: https://gist.github.com/dele256/901dd1e8f920327fc457a538996f2a29
	SDL_HitTestResult HitTestCallback(SDL_Window* Window, const SDL_Point* Area, void* Data)
	{
		int Width, Height;
		SDL_GetWindowSize(Window, &Width, &Height);

		if (Area->y < MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_TOPRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_TOP;
			}
		}
		else if (Area->y > Height - MOUSE_GRAB_PADDING)
		{
			if (Area->x < MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMLEFT;
			}
			else if (Area->x > Width - MOUSE_GRAB_PADDING)
			{
				return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
			}
			else
			{
				return SDL_HITTEST_RESIZE_BOTTOM;
			}
		}
		else if (Area->x < MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_LEFT;
		}
		else if (Area->x > Width - MOUSE_GRAB_PADDING)
		{
			return SDL_HITTEST_RESIZE_RIGHT;
		}
		else if (Area->y < WINDOW_DRAG_PADDING)
		{
			return SDL_HITTEST_DRAGGABLE;
		}
		
		return SDL_HITTEST_NORMAL; // SDL_HITTEST_NORMAL <- Windows behaviour
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

	void SDLPlatformWindow::SetBorderless(bool borderless)
	{
		SDL_SetWindowBordered(handle, borderless ? SDL_FALSE : SDL_TRUE);

		if (borderless)
		{
			SDL_SetWindowHitTest(handle, HitTestCallback, nullptr);
			SDL_SetWindowTitle(handle, "");
		}
		else
		{
			SDL_SetWindowHitTest(handle, nullptr, nullptr);
		}
	}

	bool SDLPlatformWindow::IsBorderless()
	{
		return (SDL_GetWindowFlags(handle) & SDL_WINDOW_BORDERLESS) != 0;
	}

	bool SDLPlatformWindow::IsMinimized()
	{
		return (SDL_GetWindowFlags(handle) & SDL_WINDOW_MINIMIZED) != 0;
	}

	bool SDLPlatformWindow::IsFullscreen()
	{
		return (SDL_GetWindowFlags(handle) & SDL_WINDOW_FULLSCREEN) != 0;
	}

	bool SDLPlatformWindow::IsShown()
	{
		return (SDL_GetWindowFlags(handle) & SDL_WINDOW_SHOWN) != 0;
	}

	bool SDLPlatformWindow::IsHidden()
	{
		return (SDL_GetWindowFlags(handle) & SDL_WINDOW_HIDDEN) != 0;
	}

	VkSurfaceKHR SDLPlatformWindow::CreateVulkanSurface(VkInstance instance)
	{
		VkSurfaceKHR surface = nullptr;
		SDL_Vulkan_CreateSurface(handle, instance, &surface);
		return surface;
	}

	u64 SDLPlatformWindow::GetWindowId()
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

