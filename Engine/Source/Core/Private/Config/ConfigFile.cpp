
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
#if PAL_TRAIT_BUILD_EDITOR
        IO::Path configPath = ProjectSettings::Get().GetEditorProjectDirectory() / "Config" / fileName;
#else
        IO::Path configPath = PlatformDirectories::GetAppRootDir() / "Config" / fileName;
#endif

        if (!configPath.Exists())
            configPath = PlatformDirectories::GetAppRootDir() / "Config" / fileName;
        if (!configPath.Exists())
            return;
        
        IO::FileStream inFile = IO::FileStream(configPath.GetString(), IO::OpenMode::ModeRead);
        if (!inFile.IsOpen())
            return;
        
        auto len = inFile.GetLength();
        char* buffer = new char[len + 1];
        char* bufferPtr = buffer;
        inFile.Read(len, buffer);
        inFile.Close();
        
        int startIdx = 0;
        Name currentSection{};

        for (int i = 0; i < len; i++)
        {
            if (buffer[i] == '\n')
            {
                auto view = StringView(buffer + startIdx, i - startIdx);
                if (view.IsEmpty())
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
                            auto& section = this->Get(currentSection);

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
                                    section[keyView] = ConfigValue(valueView);
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

} // namespace CE

