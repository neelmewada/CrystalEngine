#pragma once

namespace CE::Editor
{

	class EDITORCORE_API WindowsEditorPlatform : public EditorPlatformBase
	{
		CE_STATIC_CLASS(WindowsEditorPlatform)
	public:
		
		static IO::Path ShowSelectDirectoryDialog(const IO::Path& defaultPath);

		static IO::Path ShowFileSelectionDialog(const IO::Path& defaultPath, const Array<FileType>& fileTypes);

	};

	typedef WindowsEditorPlatform EditorPlatform;
    
} // namespace CE::Editor
