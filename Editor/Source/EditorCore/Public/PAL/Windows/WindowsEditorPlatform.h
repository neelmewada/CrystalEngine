#pragma once

namespace CE::Editor
{

	class EDITORCORE_API WindowsEditorPlatform : public EditorPlatformBase
	{
		CE_STATIC_CLASS(WindowsEditorPlatform)
	public:
		
		static IO::Path ShowSelectDirectoryDialog(const IO::Path& defaultPath);

	};

	typedef WindowsEditorPlatform EditorPlatform;
    
} // namespace CE::Editor
