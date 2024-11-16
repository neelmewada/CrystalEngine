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

		static const String& GetEditorExecutableName()
		{
			static String name = "EditorLauncher";
			return name;
		}

	};

}

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsEditorPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacEditorPlatform.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxEditorPlatform.h"
#endif
