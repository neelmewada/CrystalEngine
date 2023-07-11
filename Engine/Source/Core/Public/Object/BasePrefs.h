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

		void SetString(const String& key, const String& string);
		void SetFloat(const String& key, f32 number);
		void SetInt(const String& key, int number);
		void SetBool(const String& key, bool boolean);
		void SetNull(const String& key);

		String GetString(const String& key, const String& defaultValue = "");
		f32 GetFloat(const String& key, f32 defaultValue = 0);
		int GetInt(const String& key, int defaultValue = 0);
		bool GetBool(const String& key, bool defaultValue = false);

		bool IsStringValue(const String& key);
		bool IsNumberValue(const String& key);
		bool IsBooleanValue(const String& key);
		bool IsObjectValue(const String& key);
		bool IsArrayValue(const String& key);

		JsonValueType GetValueType(const String& key);

		bool GetStruct(const String& key, StructType* type, void* instance);
		bool SetStruct(const String& key, StructType* type, void* instance);

		template<typename TStruct> requires std::is_class_v<TStruct>
		inline bool GetStruct(const String& key, TStruct* instance)
		{
			return GetStruct(key, TStruct::Type(), instance);
		}

		template<typename TStruct> requires std::is_class_v<TStruct>
		inline bool SetStruct(const String& key, TStruct* instance)
		{
			return SetStruct(key, TStruct::Type(), instance);
		}

		/// Loads prefs from disk
		virtual void LoadPrefs() = 0;

		/// Saves prefs to disk
		virtual void SavePrefs() = 0;

		/// Clears and saves empty prefs to disk
		void DeletePrefs();

		/// Clears all prefs in memory. Changes won't be saved to disk automatically.
		void ClearAll();

	protected:

		virtual bool LoadPrefs(const IO::Path& prefsPath);
		virtual void SavePrefs(const IO::Path& prefsPath);
		virtual void ClearPrefs(const IO::Path& prefsPath);

		JValue json;
	};

} // namespace CE


