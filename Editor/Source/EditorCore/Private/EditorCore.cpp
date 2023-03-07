
#include "EditorCore.h"

CE_IMPLEMENT_MODULE(EditorCore, CE::Editor::EditorCoreModule)

namespace CE::Editor
{

	void EditorCoreModule::StartupModule()
	{

	}

	void EditorCoreModule::ShutdownModule()
	{

	}

	void EditorCoreModule::RegisterTypes()
	{
		CE_REGISTER_TYPES(
			Qt::CEQtApplication
		);
	}

} // namespace CE::Editor
