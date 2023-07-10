#include "CoreMinimal.h"

namespace CE
{
	BasePrefs::BasePrefs()
	{

	}

	BasePrefs::~BasePrefs()
	{
		delete rootJson;
		rootJson = nullptr;
	}

	bool BasePrefs::KeyExists(const String& key)
	{
		return rootJson->IsObjectValue() && rootJson->KeyExists(key);
	}

	bool BasePrefs::GetStringValue(const String& key, String& outString)
	{
		auto& json = *rootJson;

		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		if (!json[key].IsStringValue())
			return false;

		outString = json[key].GetStringValue();
		return true;
	}

	bool BasePrefs::GetNumberValue(const String& key, f32& outNumber)
	{
		auto& json = *rootJson;

		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		if (!json[key].IsNumberValue())
			return false;

		outNumber = (f32)json[key].GetNumberValue();
		return true;
	}

	bool BasePrefs::GetBooleanValue(const String& key, bool& outBoolean)
	{
		auto& json = *rootJson;

		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		if (!json[key].IsBoolValue())
			return false;

		outBoolean = json[key].GetBoolValue();
		return true;
	}

	bool BasePrefs::IsNullValue(const String& key)
	{
		auto& json = *rootJson;
		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		return json[key].IsNullValue();
	}

	bool BasePrefs::IsObjectValue(const String& key)
	{
		auto& json = *rootJson;
		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		return json[key].IsObjectValue();
	}

	bool BasePrefs::IsArrayValue(const String& key)
	{
		auto& json = *rootJson;
		if (!json.IsObjectValue() || !json.KeyExists(key))
			return false;
		return json[key].IsArrayValue();
	}

	JsonValueType BasePrefs::GetValueType(const String& key)
	{
		auto& json = *rootJson;
		if (!json.IsObjectValue() || !json.KeyExists(key))
			return JsonValueType::None;
		return json.GetValueType();
	}

	bool BasePrefs::LoadObject(const String& key, StructType* type, void* instance)
	{
		auto& json = *rootJson;
		if (!json.IsObjectValue() || !json.KeyExists(key) || !json[key].IsObjectValue())
			return false;
		
		auto& object = json[key].GetObjectValue();


	}


	void BasePrefs::LoadPrefs(const IO::Path& prefsPath)
	{
		delete rootJson;
		rootJson = nullptr;

		if (!prefsPath.Exists())
		{
			String string = "{\n}";
			MemoryStream stream = MemoryStream(string.GetCString(), string.GetLength());
			stream.SetAsciiMode(true);
			rootJson = JsonSerializer::Deserialize(&stream);
			return;
		}

		FileStream fileStream = FileStream(prefsPath, Stream::Permissions::ReadOnly);
		fileStream.SetAsciiMode(true);
		bool canRead = fileStream.CanRead();
		rootJson = JsonSerializer::Deserialize(&fileStream);

		if (rootJson == nullptr)
			rootJson = new JsonValue(JsonObject());
	}

	void BasePrefs::SavePrefs(const IO::Path& prefsPath)
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

} // namespace CE

