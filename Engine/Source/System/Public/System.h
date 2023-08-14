#pragma once

#include "Core.h"

#include "Asset/AssetData.h"
#include "Asset/AssetRegistry.h"

#include "Engine/AssetManager.h"

// Assets
#include "Engine/TextureDefines.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"

#include "Engine/Engine.h"

namespace CE
{

	/*
	*	Globals
	*/

	extern SYSTEM_API Engine* gEngine;

	SYSTEM_API JobManager* GetJobManager();
}
