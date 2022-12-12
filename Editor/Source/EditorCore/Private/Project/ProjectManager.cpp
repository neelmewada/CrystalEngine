
#include "Project/ProjectManager.h"

namespace CE::Editor
{

	ProjectManager::ProjectManager()
	{

	}

	ProjectManager::~ProjectManager()
	{

	}

	void ProjectManager::LoadProject(IO::Path projectPath)
	{
		loadedProjectPath = projectPath;
		isLoaded = true;
	}

	void ProjectManager::UnloadProject()
	{
		isLoaded = false;
		loadedProjectPath = "";
	}

} // namespace CE::Editor
