
#include "CoreMinimal.h"

#define MINI_CASE_SENSITIVE
#include "mini/ini.h"

namespace CE
{

	CORE_API ConfigType CFG_Engine()
	{
		return ConfigType("Engine", false);
	}

	CORE_API ConfigType CFG_Editor()
	{
		return ConfigType("Editor", false);
	}

	CORE_API ConfigType CFG_Game()
	{
		return ConfigType("Game", false);
	}

#if PAL_TRAIT_BUILD_TESTS
	CORE_API ConfigType CFG_Test()
	{
		return ConfigType("Test", false);
	}
#endif
    
    ConfigFile::ConfigFile()
    {
        
    }

    ConfigFile::~ConfigFile()
    {
        
    }

    void ConfigFile::Read(const IO::Path& configPath)
    {
        IO::FileStream inFile = IO::FileStream(configPath.GetString(), IO::OpenMode::ModeRead);
        if (!inFile.IsOpen())
            return;
        
        auto len = inFile.GetLength();
        char* buffer = new char[len + 1];
        memset(buffer, 0, len + 1);
        char* bufferPtr = buffer;
        inFile.Read(len, buffer);
        inFile.Close();
        
        int startIdx = 0;
        Name currentSection{};

        for (int i = 0; i < len; i++)
        {
            bool isLast = i == len - 1;
            if (buffer[i] == '\n' || (!isLast && buffer[i] == '\r' && buffer[i + 1] == '\n') || isLast)
            {
                if (isLast && buffer[i] != '\n' && buffer[i] != '\r') i++;
                
                auto view = StringView(buffer + startIdx, i - startIdx);
                if (view.IsEmpty() || view.StartsWith(";")) // ; Comment
                {
                    startIdx = i + 1;
                    continue;
                }
                
                if (view.StartsWith("[")) // [Section]
                {
                    currentSection = String(view.GetSubstringView(1, view.GetLength() - 2));
                    
                    if (!this->KeyExists(currentSection))
                        this->Add({ currentSection, {} });
                }
                else // Key=Value
                {
                    int splitIdx = -1;
                    for (int j = startIdx; j < i; j++)
                    {
                        if (buffer[j] == '=')
                        {
                            splitIdx = j;
                            break;
                        }
                    }

                    if (splitIdx > 0)
                    {
                        auto keyView = StringView(buffer + startIdx, splitIdx - startIdx);
                        auto valueView = StringView(buffer + splitIdx + 1, i - splitIdx - 1);
                        
                        if (currentSection.IsValid() && this->SectionExists(currentSection))
                        {
                            auto& section = Super::operator[](currentSection);

                            bool isPlus = keyView.StartsWith("+");
                            bool isMinus = keyView.StartsWith("-");
                            bool isClear = keyView.StartsWith("!");
                            bool noPrefix = !isPlus && !isMinus && !isClear;

                            if (!noPrefix) // Prefix present
                            {
                                keyView = keyView.GetSubstringView(1);
                            }

                            bool keyAlreadyExists = section.KeyExists(keyView);

                            if (isPlus)
                            {
                                if (!keyAlreadyExists)
                                {
                                    section.Add({ String(keyView), ConfigValue(Array<String>({ valueView })) });
                                }
                                else
                                {
                                    auto& ref = section[keyView];
                                    ref.SetAsArray();
                                    ref.AddValue(valueView);
                                }
                            }
                            else if (isMinus)
                            {
                                if (!keyAlreadyExists)
                                {
                                    section.Add({ String(keyView), ConfigValue(Array<String>({ valueView })) });
                                }
                                else
                                {
                                    auto& ref = section[keyView];
                                    ref.SetAsArray();
                                    ref.RemoveValue(valueView);
                                }
                            }
                            else if (isClear)
                            {
                                if (keyAlreadyExists)
                                {
                                    auto& ref = section[keyView];
                                    ref.SetAsString();
                                    ref.GetArray().Clear();
                                }
                            }
                            else
                            {
                                if (!keyAlreadyExists)
                                    section.Add({ String(keyView), ConfigValue(valueView) });
                                else
                                {
                                    section[keyView].SetAsString();
                                    section[keyView].GetArray().Clear();
                                    section[keyView].GetArray().Add(valueView);
                                }
                            }
                        }
                    }
                }

                startIdx = i + 1;
                continue;
            }
        }
        
        buffer[len] = 0;
        delete bufferPtr;
    }

    /*
     *  Config Cache
     */

    ConfigCache::ConfigCache()
    {
        
    }

    ConfigCache::~ConfigCache()
    {
        Clear();
    }

    String ConfigCache::FindBaseConfigFileName(ConfigType type)
    {
        int count = COUNTOF(gConfigTiers);
        auto engineRootDir = PlatformDirectories::GetEngineRootDir();
        
        for (int i = 0; i < count; i++)
        {
            if (gConfigTiers[i].displayName == "Base")
            {
                return String::Format(gConfigTiers[i].relativePath,
                    String::Arg("ENGINE", engineRootDir),
                    String::Arg("TYPE", type.name.GetString()));
            }
        }
        
        return "";
    }

    String ConfigCache::FindBaseConfigFileName(ConfigType type, String platform)
    {
        int count = COUNTOF(gConfigTiers);
        auto engineRootDir = PlatformDirectories::GetEngineRootDir();
        
        for (int i = 0; i < count; i++)
        {
            if (gConfigTiers[i].displayName == "BasePlatform")
            {
                return String::Format(gConfigTiers[i].relativePath,
                    String::Arg("ENGINE", engineRootDir),
                    String::Arg("TYPE", type.name.GetString()),
                    String::Arg("PLATFORM", platform));
            }
        }
        
        return "";
    }

    void ConfigCache::LoadStartupConfigs()
    {
        gConfigCache->LoadConfig(CFG_Engine());
        gConfigCache->LoadConfig(CFG_Game());
#if PAL_TRAIT_BUILD_EDITOR
        gConfigCache->LoadConfig(CFG_Editor());
#endif
    }

    void ConfigCache::LoadConfig(const ConfigType& type)
    {
        if (cache.KeyExists(type))
            return;

        auto engineRootDir = PlatformDirectories::GetEngineRootDir();
        auto projectRootDir = gProjectPath;
        auto platformName = PlatformMisc::GetPlatformName();

        int layerCount = COUNTOF(gConfigTiers);

        ConfigFile* file = new ConfigFile;

        for (int i = 0; i < layerCount; i++)
        {
            IO::Path configPath = String::Format(gConfigTiers[i].relativePath,
                String::Arg("ENGINE", engineRootDir),
                String::Arg("TYPE", type.name.GetString()),
                String::Arg("PLATFORM", platformName),
                String::Arg("PROJECT", projectRootDir));

            if (!configPath.Exists())
                continue;
            file->Read(configPath);
        }

        cache[type] = file;
    }

    Array<IO::Path> ConfigCache::GetConfigPaths(const ConfigType& type)
    {
        int layerCount = COUNTOF(gConfigTiers);
        Array<IO::Path> result{};

        auto engineRootDir = PlatformDirectories::GetEngineRootDir();
        auto projectRootDir = gProjectPath;
        auto platformName = PlatformMisc::GetPlatformName();
        
        for (int i = 0; i < layerCount; i++)
        {
            IO::Path configPath = String::Format(gConfigTiers[i].relativePath,
                String::Arg("ENGINE", engineRootDir),
                String::Arg("TYPE", type.name.GetString()),
                String::Arg("PLATFORM", platformName),
                String::Arg("PROJECT", projectRootDir));

            result.Add(configPath);
        }

        return result;
    }

    void ConfigCache::Clear()
    {
        for (auto [configType, configFile] : cache)
        {
            delete configFile;
        }
        
        cache.Clear();
    }

    ConfigFile* ConfigCache::GetConfigFile(const ConfigType& type)
    {
        if (!cache.KeyExists(type))
        {
            LoadConfig(type);
        }

        if (!cache.KeyExists(type))
            return nullptr;

        return cache[type];
    }
    
} // namespace CE

