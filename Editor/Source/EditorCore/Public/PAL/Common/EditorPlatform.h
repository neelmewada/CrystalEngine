#pragma once

namespace CE::Editor
{

	class EDITORCORE_API EditorPlatformBase
	{
	public:

		struct FileType
		{
			String desc{};
			Array<String> extensions{};
		};

	};

}

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsEditorPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacEditorPlatform.h"
#endif
