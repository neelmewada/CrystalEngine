#pragma once

namespace CE::Editor
{

	class EDITORCORE_API EditorPrefs : public BasePrefs
	{
		EditorPrefs();
		~EditorPrefs();
	public:
		typedef BasePrefs Super;

		static EditorPrefs& Get()
		{
			static EditorPrefs instance{};
			return instance;
		}

		void LoadPrefs() override;
		void SavePrefs() override;

	private:

	};

} // namespace CE::Editor

