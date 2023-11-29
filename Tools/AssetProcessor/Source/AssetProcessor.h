#pragma once

#include "CoreMinimal.h"
#include "CoreMedia.h"
#include "CoreShader.h"
#include "System.h"
#include "EditorCore.h"

class AssetProcessor
{
public:

	AssetProcessor(int argc, char** argv);

	~AssetProcessor();

	int Run();

	void Initialize();

	void Shutdown();

};
