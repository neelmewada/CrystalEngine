#include "EditorCore.h"

namespace CE::Editor
{

    bool ProjectManager::LoadProject(const IO::Path& projectFilePath)
    {
        if (!projectFilePath.Exists() || projectFilePath.GetExtension().GetString() != ".cproj")
            return false;

        CrystalProject project{};
        JsonFieldDeserializer deserializer = JsonFieldDeserializer(CrystalProject::Type(), &project);
        
        FileStream stream = FileStream(projectFilePath);
        stream.SetAsciiMode(true);
        
        while (deserializer.HasNext())
        {
            bool result = deserializer.ReadNext(&stream);
        }

		isProjectOpen = true;
		currentProject = project;
		
		return true;
    }

    bool ProjectManager::CreateEmptyProject(const IO::Path& projectFolder, const String& projectName)
    {
        if (!projectFolder.IsDirectory() || projectName.IsEmpty())
            return false;
		if (projectFolder.Exists()) 
			IO::Path::RemoveRecursively(projectFolder);
		IO::Path::CreateDirectories(projectFolder);
		if (!projectName.EndsWith(".cproj"))
			const_cast<String&>(projectName) += ".cproj";
        
        CrystalProject project{};
        project.projectName = projectName;
        project.engineVersion = CE_ENGINE_VERSION_STRING_SHORT;
        
        FileStream stream = FileStream(projectFolder / projectName, Stream::Permissions::WriteOnly);
        stream.SetAsciiMode(true);
        
        JsonFieldSerializer serializer = JsonFieldSerializer(CrystalProject::Type(), &project);
        
        while (serializer.HasNext())
        {
            serializer.WriteNext(&stream);
        }

		if (!(projectFolder / "Game").Exists())
		{
			IO::Path::CreateDirectories(projectFolder / "Game");
		}
		if (!(projectFolder / "Config").Exists())
		{
			IO::Path::CreateDirectories(projectFolder / "Config");
		}
		if (!(projectFolder / "Logs").Exists())
		{
			IO::Path::CreateDirectories(projectFolder / "Logs");
		}

		// Config files
		{
			{
				FileStream defaultEngineIni{ projectFolder / "Config" / "DefaultEngine.ini", Stream::Permissions::WriteOnly };
				defaultEngineIni.SetAsciiMode(true);
				defaultEngineIni << "; Default engine settings for project\n\n";
				defaultEngineIni.Close();
			}

			{
				FileStream defaultEditorIni = FileStream(projectFolder / "Config" / "DefaultEditor.ini", Stream::Permissions::WriteOnly);
				defaultEditorIni.SetAsciiMode(true);
				defaultEditorIni << "; Default editor settings for project\n\n";
				defaultEditorIni.Close();
			}

			{
				FileStream defaultProjectIni = FileStream(projectFolder / "Config" / "DefaultGame.ini", Stream::Permissions::WriteOnly);
				defaultProjectIni.SetAsciiMode(true);
				defaultProjectIni << "; Default game/project settings for project\n\n";
				defaultProjectIni.Close();
			}
		}
		
        return true;
    }
}
