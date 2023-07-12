#include "EditorCore.h"

namespace CE::Editor
{

	EditorPrefs::EditorPrefs()
	{
		
	}

	EditorPrefs::~EditorPrefs()
	{

	}

	void EditorPrefs::LoadPrefs()
	{
		auto appDataPath = PlatformDirectories::GetEditorAppDataDir();
		if (!appDataPath.Exists())
			IO::Path::CreateDirectories(appDataPath);

		auto editorPrefsPath = appDataPath / "EditorPrefs.json";

		Super::LoadPrefs(editorPrefsPath);
	}

	void EditorPrefs::SavePrefs()
	{
		auto appDataPath = PlatformDirectories::GetEditorAppDataDir();
		if (!appDataPath.Exists())
			IO::Path::CreateDirectories(appDataPath);

		auto editorPrefsPath = appDataPath / "EditorPrefs.json";
		
		Super::SavePrefs(editorPrefsPath);
	}

} // namespace CE::Editor
