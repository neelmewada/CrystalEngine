#pragma once

#include "Core.h"
#include "CoreMedia.h"
#include "CoreRHI.h"
#include "CoreShader.h"
#include "CoreApplication.h"
#include "System.h"

#include "EditorCore.h"

/////////////////////////////////////////////

#include "Editor/Widgets/EditorViewport.h"

#include "Editor/EditorEngine.h"
#include "Editor/EditorAssetManager.h"

////////////////

#include "ObjectEditor/Widgets/FieldEditor.h"
#include "ObjectEditor/Widgets/VectorFieldEditor.h"

#include "ObjectEditor/Widgets/PropertyEditorRow.h"
#include "ObjectEditor/Widgets/PropertyEditorSection.h"
#include "ObjectEditor/PropertyDrawer.h"

#include "ObjectEditor/ObjectEditor.h"

namespace CE::Editor
{
	extern EDITORSYSTEM_API EditorEngine* gEditor;

	EDITORSYSTEM_API AssetDefinitionRegistry* GetAssetDefinitionRegistry();
}