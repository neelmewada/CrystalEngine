#include "EditorCore.h"

namespace CE::Editor
{

	EditorPrefs::EditorPrefs()
	{
		
	}

	EditorPrefs::~EditorPrefs()
	{
		delete rootJson; 
		rootJson = nullptr;
	}

	void EditorPrefs::LoadPrefs()
	{
		delete rootJson;
		rootJson = nullptr;

		auto appDataPath = PlatformDirectories::GetAppDataDir();
		if (!appDataPath.Exists())
			IO::Path::CreateDirectories(appDataPath);

		auto editorPrefsPath = appDataPath / "EditorPrefs.json";

		if (!editorPrefsPath.Exists())
		{
			String string = "{\n}";
			MemoryStream stream = MemoryStream(string.GetCString(), string.GetLength());
			stream.SetAsciiMode(true);
			rootJson = JsonSerializer::Deserialize(&stream);
			return;
		}

		FileStream fileStream = FileStream(editorPrefsPath, Stream::Permissions::ReadOnly);
		fileStream.SetAsciiMode(true);
		bool canRead = fileStream.CanRead();
		rootJson = JsonSerializer::Deserialize(&fileStream);
		
		if (rootJson == nullptr)
			rootJson = new JsonValue(JsonObject());
	}

	void EditorPrefs::SavePrefs()
	{
		auto appDataPath = PlatformDirectories::GetAppDataDir();
		if (!appDataPath.Exists())
			IO::Path::CreateDirectories(appDataPath);

		auto editorPrefsPath = appDataPath / "EditorPrefs.json";
		if (editorPrefsPath.Exists())
			IO::Path::Remove(editorPrefsPath);

		FileStream fileStream = FileStream(editorPrefsPath, Stream::Permissions::WriteOnly, false);
		fileStream.SetAsciiMode(true);
		JsonSerializer::Serialize(&fileStream, rootJson);
	}

} // namespace CE::Editor
