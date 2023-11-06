#pragma once

#include "Core.h"
#include "CoreRHI.h"
#include "CoreShader.h"
#include "CoreSettings.h"

// Asset Meta
#include "Asset/AssetData.h"
#include "Asset/AssetRegistry.h"
#include "Engine/AssetManager.h"

///////////////////////////////////////////////////////
// GameFramework
///////////////////////////////////////////////////////

// Components
#include "GameFramework/ActorComponent.h"
#include "GameFramework/SceneComponent.h"
#include "GameFramework/GeometryComponent.h"
#include "GameFramework/MeshComponent.h"
#include "GameFramework/StaticMeshComponent.h"
#include "GameFramework/CameraComponent.h"

// Actors
#include "GameFramework/Actor.h"
#include "GameFramework/StaticMeshActor.h"
#include "GameFramework/CameraActor.h"

// Asset Types
#include "Engine/TextureDefines.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/Shader.h"
#include "Engine/Scene.h"
#include "Engine/StaticMesh.h"
#include "Engine/MaterialInterface.h"
#include "Engine/Material.h"
#include "Engine/MaterialAsset.h"

// Engine
#include "Engine/Subsystem.h"
#include "Engine/EngineSubsystem.h"
#include "Engine/Subsystems/SceneSubsystem.h"
#include "Engine/Subsystems/RendererSubsystem.h"
#include "Engine/GameViewport.h"
#include "Engine/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

namespace CE
{

	/*
	*	Globals
	*/

	class Engine;

	extern SYSTEM_API Engine* gEngine;

	SYSTEM_API JobManager* GetJobManager();
}
