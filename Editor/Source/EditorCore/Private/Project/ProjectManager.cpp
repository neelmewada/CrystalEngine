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
            deserializer.ReadNext(&stream);
        }
    }

    bool ProjectManager::CreateEmptyProject(const IO::Path& projectFolder, const String& projectName)
    {
        if (!projectFolder.IsDirectory() || projectName.IsEmpty())
            return false;
        if (!projectFolder.Exists())
            IO::Path::CreateDirectories(projectFolder);
        
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
        
        return true;
    }
}
