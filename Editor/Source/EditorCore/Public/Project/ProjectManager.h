#pragma once

namespace CE::Editor
{
	CLASS(Prefs = Editor)
	class EDITORCORE_API ProjectManager : public Object
	{
		CE_CLASS(ProjectManager, Object)
	private:
		ProjectManager() = default;

	protected:

		void OnAfterConstruct() override;

	public:

		virtual ~ProjectManager();

		static ProjectManager* Get();
		static ProjectManager* TryGet();

		/// Opens an existing project
        bool LoadProject(const IO::Path& projectFilePath);
        
		/// Creates an empty project without opening it
        bool CreateEmptyProject(const IO::Path& projectFolder, String projectName);

		static String GetProjectFileExtension() { return ".cproject"; }

		const Array<String>& GetRecentProjectsList() const { return recentProjects; }

		void RemoveRecentProject(const IO::Path& path);

		const CrystalProject& GetCurrentProject() const { return currentProject; }

		bool IsProjectOpen() const { return isProjectOpen; }

	private:

		FIELD(ReadOnly)
		b8 isProjectOpen = false;

		FIELD()
		CrystalProject currentProject{};

		FIELD(Prefs)
		Array<String> recentProjects{};

	};
}

#include "ProjectManager.rtti.h"