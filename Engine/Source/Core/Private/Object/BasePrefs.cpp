#include "CoreMinimal.h"

namespace CE
{
	BasePrefs::BasePrefs() : json(JObject())
	{

	}

	BasePrefs::~BasePrefs()
	{

	}

	bool BasePrefs::KeyExists(const String& key)
	{
		return json.IsObjectValue() && json.KeyExists(key);
	}

	bool BasePrefs::GetStringValue(const String& key, String& outString)
	{
		if (!KeyExists(key))
			return false;
		if (!json[key].IsStringValue())
			return false;

		outString = json[key].GetStringValue();
		return true;
	}

	bool BasePrefs::GetNumberValue(const String& key, f32& outNumber)
	{
		if (!KeyExists(key))
			return false;
		if (!json[key].IsNumberValue())
			return false;

		outNumber = (f32)json[key].GetNumberValue();
		return true;
	}

	bool BasePrefs::GetBooleanValue(const String& key, bool& outBoolean)
	{
		if (!KeyExists(key))
			return false;
		if (!json[key].IsBoolValue())
			return false;

		outBoolean = json[key].GetBoolValue();
		return true;
	}

	bool BasePrefs::IsNullValue(const String& key)
	{
		if (!KeyExists(key))
			return false;

		return json[key].IsNullValue();
	}

	void BasePrefs::SetString(const String& key, const String& string)
	{
		json[key] = JValue(string);
	}

	void BasePrefs::SetFloat(const String& key, f32 number)
	{
		json[key] = JValue(number);
	}

	void BasePrefs::SetInt(const String& key, int number)
	{
		json[key] = JValue(number);
	}

	void BasePrefs::SetBool(const String& key, bool boolean)
	{
		json[key] = JValue(boolean);
	}

	void BasePrefs::SetNull(const String& key)
	{
		json[key] = JValue(nullptr);
	}

	String BasePrefs::GetString(const String& key, const String& defaultValue)
	{
		if (!KeyExists(key))
			return defaultValue;
		if (!json[key].IsStringValue())
			return defaultValue;
		return json[key].GetStringValue();
	}

	f32 BasePrefs::GetFloat(const String& key, f32 defaultValue)
	{
		if (!KeyExists(key))
			return defaultValue;
		if (!json[key].IsNumberValue())
			return defaultValue;
		return json[key].GetNumberValue();
	}

	int BasePrefs::GetInt(const String& key, int defaultValue)
	{
		if (!KeyExists(key))
			return defaultValue;
		if (!json[key].IsNumberValue())
			return defaultValue;
		return (int)json[key].GetNumberValue();
	}

	bool BasePrefs::GetBool(const String& key, bool defaultValue)
	{
		if (!KeyExists(key))
			return defaultValue;
		if (!json[key].IsBoolValue())
			return defaultValue;
		return json[key].GetBoolValue();
	}

	bool BasePrefs::IsStringValue(const String& key)
	{
		if (!KeyExists(key))
			return false;
		return json[key].IsStringValue();
	}

	bool BasePrefs::IsNumberValue(const String& key)
	{
		if (!KeyExists(key))
			return false;
		return json[key].IsNumberValue();
	}

	bool BasePrefs::IsBooleanValue(const String& key)
	{
		if (!KeyExists(key))
			return false;
		return json[key].IsBoolValue();
	}

	bool BasePrefs::IsObjectValue(const String& key)
	{
		if (!KeyExists(key))
			return false;
		return json[key].IsObjectValue();
	}

	bool BasePrefs::IsArrayValue(const String& key)
	{
		if (!KeyExists(key))
			return false;
		return json[key].IsArrayValue();
	}

	JsonValueType BasePrefs::GetValueType(const String& key)
	{
		if (!KeyExists(key))
			return JsonValueType::None;

		if (json[key].IsNumberValue())
			return JsonValueType::Number;
		else if (json[key].IsBoolValue())
			return JsonValueType::Boolean;
		else if (json[key].IsStringValue())
			return JsonValueType::String;
		else if (json[key].IsArrayValue())
			return JsonValueType::Array;
		else if (json[key].IsObjectValue())
			return JsonValueType::Object;
		else if (json[key].IsNullValue())
			return JsonValueType::Null;
		
		return JsonValueType::None;
	}

	bool BasePrefs::GetStruct(const String& key, StructType* type, void* instance)
	{
		if (key.IsEmpty())
			return false;
		if (!json.KeyExists(key))
			return false;

		const JValue& value = json[key];

		JsonFieldDeserializer deserializer{ type, instance };

		while (deserializer.HasNext())
		{
			deserializer.ReadNext(value);
		}

		return true;
	}

	bool BasePrefs::SetStruct(const String& key, StructType* type, void* instance)
	{
		if (key.IsEmpty())
			return false;

		if (json.KeyExists(key))
		{
			json.GetObjectValue().Remove(key);
		}

		json.GetObjectValue().Add({ key, JObject() });
		JValue& value = json[key];

		JsonFieldSerializer serializer{ type, instance };

		while (serializer.HasNext())
		{
			serializer.WriteNext(value);
		}

		return true;
	}

	void BasePrefs::DeletePrefs()
	{
		ClearAll();
		SavePrefs();
	}

	void BasePrefs::ClearAll()
	{
		json = JValue(JObject());
	}


	bool BasePrefs::LoadPrefs(const IO::Path& prefsPath)
	{
		if (!prefsPath.Exists())
		{
			json = JObject();
			return false;
		}

		FileStream fileStream = FileStream(prefsPath, Stream::Permissions::ReadOnly);
		bool success = JsonSerializer::Deserialize2(&fileStream, json);
		fileStream.Close();

		return success;
	}

	void BasePrefs::SavePrefs(const IO::Path& prefsPath)
	{
		if (prefsPath.Exists())
		{
			IO::Path::Remove(prefsPath);
		}

		auto parentPath = prefsPath.GetParentPath();
		if (!parentPath.Exists())
		{
			IO::Path::CreateDirectories(parentPath);
		}

		FileStream fileStream = FileStream(prefsPath, Stream::Permissions::WriteOnly, false);
		fileStream.SetAsciiMode(true);

		JsonSerializer::Serialize2(&fileStream, json);

		fileStream.Close();
	}

	void BasePrefs::ClearPrefs(const IO::Path& prefsPath)
	{
		json = JObject();
		SavePrefs(prefsPath);
	}

} // namespace CE

