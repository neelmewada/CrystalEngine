#pragma once

namespace CE
{

	class WindowsWindowMisc
	{
		CE_STATIC_CLASS(WindowsWindowMisc)
	public:

		static u32 GetDpiForWindow(SDLPlatformWindow* window);

	};

	typedef WindowsWindowMisc PlatformWindowMisc;

} // namespace CE
