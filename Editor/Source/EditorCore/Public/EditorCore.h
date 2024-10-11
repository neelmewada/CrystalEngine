#pragma once

#include "Core.h"
#include "CoreRHI.h"
#include "System.h"

#include "EditorGlobals.h"

// Platform
#include "PAL/Common/EditorPlatform.h"

// Project
#include "Project/CrystalProject.h"
#include "Project/ProjectManager.h"

// Asset
#include "Asset/AssetDefinition.h"
#include "Asset/AssetDefinitionRegistry.h"
#include "Asset/AssetImporter.h"

// Assets
#include "Asset/ShaderAssetDefinition.h"
#include "Asset/ShaderAssetImporter.h"
#include "Asset/TextureAssetDefinition.h"
#include "Asset/TextureAssetImporter.h"
#include "Asset/StaticMeshDefinition.h"
#include "Asset/StaticMeshAssetImporter.h"
#include "Asset/FontAssetDefinition.h"
#include "Asset/FontAssetImporter.h"

///////////////////////////////////////////////////////
/// Widgets

#include "Fusion/Style/EditorDockspaceStyle.h"
#include "Fusion/Style/EditorMinorDockspaceStyle.h"
#include "Fusion/Style/EditorDockTabStyle.h"
#include "Fusion/Style/EditorMenuBarStyle.h"
#include "Fusion/Style/EditorStyle.h"

#include "Fusion/Widget/EditorToolBar.h"
#include "Fusion/Widget/EditorMenuBar.h"
#include "Fusion/Widget/EditorMenuPopup.h"

#include "Fusion/Editor/EditorWindow.h"
#include "Fusion/Editor/EditorDockspace.h"
#include "Fusion/Editor/EditorDockTabItem.h"
#include "Fusion/Editor/EditorDockTab.h"

#include "Fusion/Editor/EditorMinorDockspace.h"

namespace CE::Editor
{
    
} // namespace CE::Editor


