
#include "CoreMinimal.h"

#include "Project/ProjectSettings.h"

namespace CE
{
	ProjectSettings ProjectSettings::currentProject = {};

	ProjectSettings::ProjectSettings()
	{
		LoadProjectSettings();
	}

	void ProjectSettings::LoadProjectSettings()
	{
#if !PAL_TRAIT_BUILD_EDITOR
		// Standalone game build
		CE::SerializedObject so{ Self::Type(), this };

		auto filePath = PlatformDirectories::GetGameDir() / "Game.dat";

		if (!filePath.Exists())
		{
			CE_LOG(Error, All, "Failed to LoadProjectSettings(). Couldn't find file at path: {}", filePath);
			return;
		}

		IO::FileStream file{ filePath, IO::OpenMode::ModeRead };
		so.Deserialize(file);

		loaded = true;
#endif
	}

	ProjectSettings& ProjectSettings::Get()
	{
		static ProjectSettings project{};
		return project;
	}

} // namespace CE

CE_RTTI_STRUCT_IMPL(SYSTEM_API, CE, ProjectSettings)
