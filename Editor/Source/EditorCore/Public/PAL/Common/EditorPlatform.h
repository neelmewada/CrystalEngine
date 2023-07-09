#pragma once

namespace CE::Editor
{

	class EDITORCORE_API EditorPlatformBase
	{
		
	};

}

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsEditorPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Windows/MacEditorPlatform.h"
#endif
