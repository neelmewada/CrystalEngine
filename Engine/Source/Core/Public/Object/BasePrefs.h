#pragma once

using json = nlohmann::json;

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

		bool GetStruct(const String& key, StructType* type, void* instance);
		bool SetStruct(const String& key, StructType* type, void* instance);

		template<typename TStruct>
		inline bool GetStruct(const String& key, TStruct* instance)
		{
			return GetStruct(key, TStruct::Type(), instance);
		}

		template<typename TStruct>
		inline bool SetStruct(const String& key, TStruct* instance)
		{
			return SetStruct(key, TStruct::Type(), instance);
		}

		virtual void LoadPrefs() = 0;
		virtual void SavePrefs() = 0;

		virtual void ClearPrefs();

	protected:

		void ReadField(const json& jsonData, FieldType* field, void* instance);
		void WriteField(json& jsonData, FieldType* field, void* instance);

		virtual void LoadPrefs(const IO::Path& prefsPath);
		virtual void SavePrefs(const IO::Path& prefsPath);
		virtual void ClearPrefs(const IO::Path& prefsPath);

		json data{};
	};

} // namespace CE


