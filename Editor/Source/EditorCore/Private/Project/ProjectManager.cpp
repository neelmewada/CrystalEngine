
#include "Project/ProjectManager.h"

#include "System.h"

namespace CE::Editor
{

	ProjectManager::ProjectManager()
	{

	}

	ProjectManager::~ProjectManager()
	{

	}

	void ProjectManager::CreateProject(IO::Path projectDirectory, String projectName)
	{
		if (!projectDirectory.Exists())
		{
			IO::Path::CreateDirectories(projectDirectory);
		}

		ProjectSettings* projectSettings = &ProjectSettings::Get();
		projectSettings->projectName = projectName;
		projectSettings->projectVersion = "0.0.1";
		projectSettings->engineVersion = CE_ENGINE_VERSION_STRING;

		auto projectPath = projectDirectory / projectName / (projectName + ".cproject");

		SerializedObject projectSettingsSO{ ProjectSettings::Type(), projectSettings };
		projectSettingsSO.Serialize(projectPath);

		loadedProjectPath = projectPath;
		isLoaded = true;
	}

	void ProjectManager::LoadProject(IO::Path projectPath)
	{
		if (projectPath.IsDirectory() || !projectPath.Exists())
		{
			return;
		}

		ProjectSettings* projectSettings = &ProjectSettings::Get();
		
		SerializedObject projectSettingsSO{ ProjectSettings::Type(), projectSettings };
		projectSettingsSO.Deserialize(projectPath);

		loadedProjectPath = projectPath;
		isLoaded = true;
	}

	void ProjectManager::UnloadProject()
	{
		ProjectSettings::currentProject = ProjectSettings();

		loadedProjectPath = {};
		isLoaded = false;
	}

	ProjectSettings& ProjectManager::GetCurrentProject()
	{
		return ProjectSettings::Get();
	}

} // namespace CE::Editor
