
#include "CoreMinimal.h"

#define MINI_CASE_SENSITIVE
#include "mini/ini.h"

namespace CE
{
    
    ConfigFile::ConfigFile()
    {
        
    }

    ConfigFile::~ConfigFile()
    {
        
    }

    void ConfigFile::Read(const String& fileName)
    {
        IO::Path configPath = PlatformDirectories::GetEngineDir().GetParentPath() / "Config" / fileName;
        
        if (!configPath.Exists())
            configPath = ProjectSettings::Get().GetEditorProjectDirectory() / "Config" / fileName;
        if (!configPath.Exists())
            return;
        
        IO::FileStream inFile = IO::FileStream(configPath.GetString(), IO::OpenMode::ModeRead);
        if (!inFile.IsOpen())
            return;
        
        auto len = inFile.GetLength();
        char* buffer = new char[len + 1];
        inFile.Read(len, buffer);
        inFile.Close();
        
        int i = 0;
        int startIdx = 0;
        for (int i = 0; i < len; i++)
        {
            if (buffer[i] == '\n')
            {
                auto view = StringView(buffer + startIdx, i - startIdx);
                view.GetSize();
                startIdx = i + 1;
                continue;
            }
        }
        
        buffer[len + 1] = 0;
        delete buffer;
        
        mINI::INIFile file{configPath.GetString().ToStdString()};
        
        mINI::INIStructure ini;
        file.read(ini);
        
        for (const auto& [sectionName, sectionMap] : ini)
        {
            Name name = Name(sectionName);
            if (!this->KeyExists(name))
                this->Add({ name, {} });
            
            for (const auto& [key, value] : sectionMap)
            {
                this->Get(name).Add({ key, ConfigValue(value) });
            }
        }
    }

} // namespace CE

