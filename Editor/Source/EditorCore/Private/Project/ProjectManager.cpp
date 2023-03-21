
#include "Project/ProjectManager.h"

#include "System.h"

namespace CE::Editor
{

	ProjectManager::ProjectManager()
	{

	}

	ProjectManager::~ProjectManager()
	{
		ProjectSettings* projectSettings = &ProjectSettings::Get();
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

		// Game/Assets
		IO::Path::CreateDirectories(projectDirectory / projectName / "Game/Assets");

		projectSettings->editorProjectDirectory = projectDirectory / projectName;

		loadedProjectPath = projectPath;
		isLoaded = true;

		AssetDatabase::Get().LoadDatabase();
	}

	void ProjectManager::LoadProject(IO::Path projectPath)
	{
		if (projectPath.IsDirectory() || !projectPath.Exists())
		{
			return;
		}
        
        CE_LOG(Info, All, "Loading project at path: {}", projectPath);

		ProjectSettings* projectSettings = &ProjectSettings::Get();
		
		SerializedObject projectSettingsSO{ ProjectSettings::Type(), projectSettings };
		projectSettingsSO.Deserialize(projectPath);

		projectSettings->editorProjectDirectory = projectPath.GetParentPath();

		loadedProjectPath = projectPath;
		isLoaded = true;

		AssetDatabase::Get().LoadDatabase();
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
