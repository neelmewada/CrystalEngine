#pragma once

namespace CE
{

	class CORE_API BasePrefs
	{
	protected:

		BasePrefs();
		virtual ~BasePrefs();

	public:

		bool KeyExists(const String& key);

		bool GetStringValue(const String& key, String& outString);
		bool GetNumberValue(const String& key, f32& outNumber);
		bool GetBooleanValue(const String& key, bool& outBoolean);
		bool IsNullValue(const String& key);

		bool IsObjectValue(const String& key);
		bool IsArrayValue(const String& key);

		JsonValueType GetValueType(const String& key);

		bool LoadObject(const String& key, StructType* type, void* instance);

	protected:

		virtual void LoadPrefs(const IO::Path& prefsPath);
		virtual void SavePrefs(const IO::Path& prefsPath);

		JsonValue* rootJson = nullptr;
	};

} // namespace CE


