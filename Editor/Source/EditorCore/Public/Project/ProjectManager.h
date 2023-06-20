#pragma once

namespace CE::Editor
{
	class EDITORCORE_API ProjectManager
	{
	private:
		ProjectManager() = default;

	public:

		static ProjectManager& Get()
		{
			static ProjectManager instance{};
			return instance;
		}

        bool LoadProject(const IO::Path& projectFilePath);
        
        bool CreateEmptyProject(const IO::Path& projectFolder, const String& projectName);
        
	};
}

