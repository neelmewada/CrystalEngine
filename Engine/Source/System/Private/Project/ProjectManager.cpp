
#include "Project/ProjectManager.h"

namespace CE
{

	void ProjectManager::GetCurrentProject()
	{
		project = ProjectSettings();

		auto type = ProjectSettings::Type();

		CE::SerializedObject so{ type, &project };

		
	}

} // namespace CE
