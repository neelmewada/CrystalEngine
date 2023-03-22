
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

		CE_REGISTER_TYPES(
		    DrawerBase,
		    FieldDrawer,
		    VectorFieldDrawer,
		    StringFieldDrawer
		);

		// Register Field Drawers
		CE_REGISTER_FIELD_DRAWERS(VectorFieldDrawer, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i);
		CE_REGISTER_FIELD_DRAWERS(StringFieldDrawer, String);

		// Asset Import Settings
		CE_REGISTER_TYPES(
			AssetImportSettings,
			TextureAssetImportSettings
		);

		// Register Import Settings
		CE_REGISTER_IMPORT_SETTINGS(TextureAssetImportSettings, TextureAsset, jpg, jpeg, png);
	}

} // namespace CE::Editor
