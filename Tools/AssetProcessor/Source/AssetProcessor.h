#pragma once

#include "CoreMinimal.h"
#include "CoreMedia.h"
#include "CoreShader.h"
#include "CoreRHI.h"
#include "VulkanRHI.h"
#include "System.h"
#include "EditorCore.h"

#include "cxxopts.hpp"

using namespace CE;
using namespace CE::Editor;

namespace CE
{
	class AssetProcessor
	{
	public:

		enum class AssetMode
		{
			Game = 0,
			Engine,
			Editor,
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
		void PostInit();

		void PreShutdown();
		void Shutdown();

	private:

		cxxopts::Options options{ "Asset Processor", "A command line tool to process assets." };

		cxxopts::ParseResult parsedOptions{};

		AssetMode mode{};
		ProcessMode processMode{};

		IO::Path inputRoot{};
		IO::Path outputRoot{};
		PlatformName targetPlatform{};

		Array<IO::Path> individualAssetPaths{};

		Array<IO::Path> allSourceAssetPaths{};
		Array<IO::Path> allProductAssetPaths{};

		Array<IO::Path> includePaths{};

		AssetDefinitionRegistry* assetDefRegistry = nullptr;
	};

}