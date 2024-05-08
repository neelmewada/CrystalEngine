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

namespace CE::Editor
{
	extern EDITORSYSTEM_API EditorEngine* gEditor;

	EDITORSYSTEM_API AssetDefinitionRegistry* GetAssetDefinitionRegistry();
}