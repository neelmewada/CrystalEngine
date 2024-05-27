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
#include "Editor/Widgets/EditorSplitterContainer.h"
#include "Editor/Widgets/EditorSplitter.h"

#include "Editor/Widgets/Input/NumericFieldInput.h"

#include "Editor/Tools/ColorPicker/ColorPickerGradient.h"
#include "Editor/Tools/ColorPicker/ColorPickerPreview.h"
#include "Editor/Tools/ColorPicker/ColorPickerTool.h"

#include "Editor/EditorEngine.h"
#include "Editor/EditorAssetManager.h"

////////////////

#include "ObjectEditor/Widgets/FieldEditor.h"
#include "ObjectEditor/Widgets/VectorFieldEditor.h"
#include "ObjectEditor/Widgets/NumberFieldEditor.h"
#include "ObjectEditor/Widgets/ColorInput.h"
#include "ObjectEditor/Widgets/ColorFieldEditor.h"

#include "ObjectEditor/Widgets/PropertyEditorRow.h"
#include "ObjectEditor/Widgets/PropertyEditorSection.h"
#include "ObjectEditor/PropertyDrawer.h"

#include "ObjectEditor/ObjectEditor.h"

namespace CE::Editor
{
	extern EDITORSYSTEM_API EditorEngine* gEditor;

	EDITORSYSTEM_API AssetDefinitionRegistry* GetAssetDefinitionRegistry();
}