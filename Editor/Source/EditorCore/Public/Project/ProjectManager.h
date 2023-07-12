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

		/// Opens an existing project
        bool LoadProject(const IO::Path& projectFilePath);
        
		/// Creates an empty project without opening it
        bool CreateEmptyProject(const IO::Path& projectFolder, String projectName);

		inline String GetProjectFileExtension() const { return ".cproject"; }
        
	private:

		b8 isProjectOpen = false;

		CrystalProject currentProject{};
	};
}

