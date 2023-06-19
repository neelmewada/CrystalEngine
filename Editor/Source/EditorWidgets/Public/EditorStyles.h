#pragma once

namespace CE::Editor
{

	class EDITORWIDGETS_API EditorStyles
	{
	private:
		EditorStyles() = default;
		EditorStyles(const EditorStyles&) = delete;

	public:

		static EditorStyles& Get()
		{
			static EditorStyles instance{};
			return instance;
		}

		void InitDefaultStyle();

		void GetDefaultFont(unsigned char** outFont, unsigned int* outLength);

	};
    
} // namespace CE::Editor
