#pragma once

#include "CoreMinimal.h"
#include "CoreMedia.h"
#include "CoreShader.h"
#include "System.h"
#include "EditorCore.h"

#include "cxxopts.hpp"

using namespace CE;

class AssetProcessor
{
public:

	enum class AssetMode
	{
		Game = 0,
		Engine,
		Plugin,
	};

	enum class ProcessMode
	{
		Individual,
		All,
		New
	};

	AssetProcessor(int argc, char** argv);

	~AssetProcessor();

	int Run();

	void Initialize();

	void Shutdown();

private:

	cxxopts::Options options{ "Asset Processor", "A command line tool to process assets." };

	cxxopts::ParseResult parsedOptions{};

	AssetMode mode{};
	ProcessMode processMode{};

	Array<IO::Path> individualAssetPaths{};

	IO::Path assetsDir{};

	Array<IO::Path> allSourceAssetPaths{};

	Array<IO::Path> includePaths{};

	AssetDefinitionRegistry* assetDefRegistry = nullptr;
};
