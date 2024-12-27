#pragma once

namespace CE
{

	class CORE_API Prefs final
	{
	public:

		static Prefs& Get()
		{
			static Prefs instance{};
			return instance;
		}

		void ClearAllPrefs();

		void LoadPrefsJson();

		void SavePrefsJson();

	private:

		void LoadPrefs(Object* object);
		void SavePrefs(Object* object);

		void SerializeField(const Ptr<FieldType>& field, void* instance, JValue& parent);
		void DeserializeField(const Ptr<FieldType>& field, void* instance, JValue& parent, int index = 0);

		Prefs() = default;

		static void OnClassRegistered(ClassType* clazz);
		static void OnClassDeregistered(ClassType* clazz);

		static HashMap<Name, Array<ClassType*>> prefsClasses;

		HashMap<Name, JValue> prefsJson{};

		friend class CoreModule;
		friend class Object;
	};

} // namespace CE
