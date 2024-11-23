#pragma once

namespace CE
{

	class MacWindowMisc
	{
		CE_STATIC_CLASS(MacWindowMisc)
	public:

		static u32 GetDpiForWindow(SDLPlatformWindow* window);

	};

	typedef MacWindowMisc PlatformWindowMisc;

} // namespace CE
