#include "EditorCore.h"

namespace CE::Editor
{
	static ProjectManager* instance = nullptr;

	ProjectManager::~ProjectManager()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	ProjectManager* ProjectManager::Get()
	{
		if (instance == nullptr)
		{
			instance = CreateObject<ProjectManager>(nullptr, "ProjectManager");
		}
		return instance;
	}

	ProjectManager* ProjectManager::TryGet()
	{
		return instance;
	}

	bool ProjectManager::LoadProject(const IO::Path& projectFilePath)
    {
        if (!projectFilePath.Exists() || projectFilePath.GetExtension().GetString() != GetProjectFileExtension())
            return false;

        CrystalProject project{};
        JsonFieldDeserializer deserializer = JsonFieldDeserializer(CrystalProject::Type(), &project);
        
        FileStream stream = FileStream(projectFilePath);
        stream.SetAsciiMode(true);

		deserializer.Deserialize(&stream);

		stream.Close();

		isProjectOpen = true;
		currentProject = project;

		// Set global project vars
		gProjectPath = projectFilePath.GetParentPath();
		gProjectName = projectFilePath.GetFilename().RemoveExtension().GetString();

		// Load settings package
		Package* settingsPackage = GetSettingsPackage();
		
		return true;
    }

    bool ProjectManager::CreateEmptyProject(const IO::Path& projectFolder, String projectName)
    {
		if (projectFolder.Exists())
			IO::Path::RemoveRecursively(projectFolder);
		IO::Path::CreateDirectories(projectFolder);

		String extension = GetProjectFileExtension();
		String projectFile = projectName;

        if (!projectFolder.IsDirectory() || projectFile.IsEmpty())
            return false;

		projectFile += extension;

		// Set global project path
		//gProjectPath = projectFolder;
		//gProjectName = projectName;
        
        CrystalProject project{};
        project.engineVersion = CE_ENGINE_VERSION_STRING;
        
        FileStream stream = FileStream(projectFolder / projectFile, Stream::Permissions::WriteOnly);
        stream.SetAsciiMode(true);
        
        JsonFieldSerializer serializer = JsonFieldSerializer(CrystalProject::Type(), &project);

		serializer.Serialize(&stream);
		stream.Close();
		
		IO::Path::CreateDirectories(projectFolder / "Game");
		IO::Path::CreateDirectories(projectFolder / "Game" / "Assets");

		IO::Path::CreateDirectories(projectFolder / "Config");
		IO::Path::CreateDirectories(projectFolder / "Logs");
		IO::Path::CreateDirectories(projectFolder / "Temp");

		Package* settingsPackage = GetSettingsPackage();
		
		ProjectSettings* projectSettings = GetSettings<ProjectSettings>();
		projectSettings->projectName = projectName;
		projectSettings->projectVersion = CE_ENGINE_VERSION_STRING;

		SaveSettings(projectFolder); // Saves the settings package in the global project path gProjectPath

		// Config files
		{
			{
				FileStream defaultEngineIni{ projectFolder / "Config" / "DefaultEngine.ini", Stream::Permissions::WriteOnly };
				defaultEngineIni.SetAsciiMode(true);
				defaultEngineIni << "; Default engine config for project\n\n";
				defaultEngineIni.Close();
			}

			{
				FileStream defaultEditorIni = FileStream(projectFolder / "Config" / "DefaultEditor.ini", Stream::Permissions::WriteOnly);
				defaultEditorIni.SetAsciiMode(true);
				defaultEditorIni << "; Default editor config for project\n\n";
				defaultEditorIni.Close();
			}

			{
				FileStream defaultProjectIni = FileStream(projectFolder / "Config" / "DefaultGame.ini", Stream::Permissions::WriteOnly);
				defaultProjectIni.SetAsciiMode(true);
				defaultProjectIni << "; Default game/project config for project\n\n";
				defaultProjectIni.Close();
			}
		}
		
        return true;
    }
}
