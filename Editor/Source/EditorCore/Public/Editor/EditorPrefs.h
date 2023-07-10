#pragma once

namespace CE::Editor
{

	class EDITORCORE_API EditorPrefs
	{
		EditorPrefs();
		~EditorPrefs();
	public:

		static EditorPrefs& Get()
		{
			static EditorPrefs instance{};
			return instance;
		}

		void LoadPrefs();
		void SavePrefs();

	private:

		JsonValue* rootJson = nullptr;
	};

} // namespace CE::Editor

