
#include "EditorCore.h"

#include "EditorCore.private.h"
CE_IMPLEMENT_MODULE_AUTORTTI(EditorCore, CE::Editor::EditorCoreModule)

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

		CE_REGISTER_TYPES(
		    DrawerBase,
		    FieldDrawer,
		    VectorFieldDrawer,
		    StringFieldDrawer,
			EnumFieldDrawer
		);

		// Register Field Drawers
		CE_REGISTER_FIELD_DRAWERS(VectorFieldDrawer, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i);
		CE_REGISTER_FIELD_DRAWERS(StringFieldDrawer, String);
	}

} // namespace CE::Editor