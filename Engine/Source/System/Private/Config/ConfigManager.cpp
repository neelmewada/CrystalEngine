
#include "CoreMinimal.h"

#include "Config/ConfigManager.h"

namespace CE
{

	bool ConfigManager::LoadEditorConfig(String configFileName, StructType* structType, ConfigBase* instance)
	{
		auto editorConfigDir = PlatformDirectories::GetEditorDir() / "Config";

		if (!editorConfigDir.Exists())
			return false;

		auto configFilePath = editorConfigDir / (configFileName + ".ini");

		if (!configFilePath.Exists())
			return false;

		ConfigParser parser{ structType };

		return parser.Parse(instance, configFilePath);
	}

	bool ConfigManager::LoadEngineConfig(String configFileName, StructType* structType, ConfigBase* instance)
	{
		auto engineConfigDir = PlatformDirectories::GetEngineDir() / "Config";

		if (!engineConfigDir.Exists())
			return false;

		auto configFilePath = engineConfigDir / (configFileName + ".ini");

		if (!configFilePath.Exists())
			return false;

		ConfigParser parser{ structType };

		bool result = parser.Parse(instance, configFilePath);

		auto platformConfigFilePath = engineConfigDir / "Platform" / PlatformMisc::GetPlatformName() / (configFileName + ".ini");

		if (!platformConfigFilePath.Exists())
			return result;

		parser.Parse(instance, platformConfigFilePath);

		return result;
	}

	bool ConfigManager::LoadGameConfig(String configFileName, StructType* structType, ConfigBase* instance)
	{
		auto gameConfigDir = PlatformDirectories::GetGameDir() / "Config";

		if (!gameConfigDir.Exists())
			return false;

		auto configFilePath = gameConfigDir / (configFileName + ".ini");

		if (!configFilePath.Exists())
			return false;

		ConfigParser parser{ structType };

		return parser.Parse(instance, configFilePath);
	}

} // namespace CE

