#pragma once

namespace CE
{

	class MacWindowMisc
	{
		CE_STATIC_CLASS(MacWindowMisc)
	public:

		static u32 GetDpiForWindow(SDLPlatformWindow* window);
        
        static void SetupBorderlessWindow(SDLPlatformWindow* window, bool borderless);

	};

	typedef MacWindowMisc PlatformWindowMisc;

} // namespace CE
