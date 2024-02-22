#include "AssetProcessor.h"

#define LOG(x) std::cout << x << std::endl;

namespace CE
{
	AssetProcessor::AssetProcessor(int argc, char** argv)
	{
		options.add_options()
			("h,help", "Print this help info.")
			("p,pack", "Package the assets for final distribution build. (Not implemented yet)")
			("I,include", "Include path directories for shaders, etc.", cxxopts::value<std::vector<std::string>>())
			("M,mode", "Processing mode: either Engine or Game. If assets should be processed as engine or game assets.", cxxopts::value<std::string>()->default_value("Game"))
			("i,input", "Use this flag to add input files.")
			("R,output-root", "Path to root of the assets directory.", cxxopts::value<std::string>())
			("D,input-root", "Path to root of the assets directory.", cxxopts::value<std::string>())
			("T,target", "Target platform. Values: Windows, Linux, Mac, Android, iOS", cxxopts::value<std::string>())
			;

		try
		{
			options.allow_unrecognised_options();

			parsedOptions = options.parse(argc, argv);
		
			if (parsedOptions["h"].as<bool>())
			{
				LOG(options.help());
				exit(0);
			}
		}
		catch (std::exception exc)
		{
			LOG("Failed to parse arguments: " << exc.what());
			exit(-1);
		}

		inputRoot = parsedOptions["D"].as<std::string>();
		outputRoot = parsedOptions["R"].as<std::string>();

		String targetName = parsedOptions["T"].as<std::string>();

		if (targetName == "Windows")
		{
			targetPlatform = PlatformName::Windows;
		}
		else if (targetName == "Mac")
		{
			targetPlatform = PlatformName::Mac;
		}
		else if (targetName == "Linux")
		{
			targetPlatform = PlatformName::Linux;
		}
		else if (targetName == "Android")
		{
			targetPlatform = PlatformName::Android;
		}
		else if (targetName == "iOS")
		{
			targetPlatform = PlatformName::iOS;
		}
		else
		{
			targetPlatform = PlatformMisc::GetCurrentPlatform();
		}

		String mode = parsedOptions["M"].as<std::string>();
		if (mode == "Game")
		{
			this->mode = AssetMode::Game;
		}
		else if (mode == "Engine")
		{
			this->mode = AssetMode::Engine;
		}
		else if (mode == "Editor")
		{
			this->mode = AssetMode::Editor;
		}
		else
		{
			LOG("Invalid mode passed: " << mode);
			exit(-2);
		}

		if (parsedOptions.count("I") > 0)
		{
			std::vector<std::string> includePaths = parsedOptions["I"].as<std::vector<std::string>>();
		
			for (const auto& path : includePaths)
			{
				this->includePaths.Add(path);
			}
		}

		individualAssetPaths.Clear();

		bool isMultipleFileInput = parsedOptions["i"].as<bool>();

		if (isMultipleFileInput)
		{
			processMode = ProcessMode::Individual;
			
			auto inputs = parsedOptions.unmatched();//parsedOptions["i"].as<std::vector<std::string>>();
		
			for (const auto& input : inputs)
			{
				individualAssetPaths.Add(input);
			}
		}
	
		gProjectPath = PlatformDirectories::GetLaunchDir();
		gProjectName = "AssetProcessor";
	
		allSourceAssetPaths.Clear();
		allProductAssetPaths.Clear();

		if (individualAssetPaths.NonEmpty())
		{
			allSourceAssetPaths.AddRange(individualAssetPaths);
			
			for (const auto& sourcePath : allSourceAssetPaths)
			{
				if (inputRoot != outputRoot)
				{
					IO::Path relativeSourcePath = IO::Path::GetRelative(sourcePath, inputRoot);
					IO::Path productPath = outputRoot / relativeSourcePath;
					productPath = productPath.ReplaceExtension(".casset");
					if (!productPath.GetParentPath().Exists())
					{
						IO::Path::CreateDirectories(productPath.GetParentPath());
					}
					allProductAssetPaths.Add(productPath);
				}
				else
				{
					IO::Path productPath = sourcePath.ReplaceExtension(".casset");
					allProductAssetPaths.Add(productPath);
				}
			}
		}
		else // non-individual modes
		{

		}
	}

	AssetProcessor::~AssetProcessor()
	{
	}

	int AssetProcessor::Run()
	{
		Initialize();
		PostInit();

		Logger::Initialize();
	
		JobContext* jobContext = JobContext::GetGlobalContext();
		JobManager* jobManager = jobContext->GetJobManager();

		HashMap<AssetDefinition*, Array<IO::Path>> sourcePathsByAssetDef{};
		HashMap<AssetDefinition*, Array<IO::Path>> productPathsByAssetDef{};

		for (int i = 0; i < allSourceAssetPaths.GetSize(); i++)
		{
			auto sourcePath = allSourceAssetPaths[i];
			auto productPath = allProductAssetPaths[i];
			if (!sourcePath.Exists())
				continue;

			String extension = sourcePath.GetExtension().GetString();

			AssetDefinition* assetDef = assetDefRegistry->FindAssetDefinition(extension);

			if (assetDef != nullptr)
			{
				sourcePathsByAssetDef[assetDef].Add(sourcePath);
				productPathsByAssetDef[assetDef].Add(productPath);
			}
		}

		Array<AssetImporter*> importers{};

		for (const auto& [assetDef, sourcePaths] : sourcePathsByAssetDef)
		{
			if (assetDef == nullptr || sourcePaths.IsEmpty())
				continue;
			if (!productPathsByAssetDef.KeyExists(assetDef) ||
				productPathsByAssetDef[assetDef].GetSize() != sourcePaths.GetSize())
				continue;

			ClassType* assetImporterClass = assetDef->GetAssetImporterClass();
			if (assetImporterClass == nullptr || !assetImporterClass->CanBeInstantiated())
				continue;
			auto assetImporter = CreateObject<AssetImporter>(nullptr, "AssetImporter", OF_Transient, assetImporterClass);
			if (assetImporter == nullptr)
				continue;

			assetImporter->SetIncludePaths(includePaths);
			assetImporter->SetLogging(true);
			assetImporter->SetTargetPlatform(targetPlatform);

			importers.Add(assetImporter);
			assetImporter->ImportSourceAssetsAsync(sourcePaths, productPathsByAssetDef[assetDef]);
		}

		// Wait for all jobs to complete
		jobManager->Complete();

		for (auto importer : importers)
		{
			importer->Destroy();
		}
		importers.Clear();

		Logger::Shutdown();

		PreShutdown();
		Shutdown();

		return 0;
	}

	void AssetProcessor::Initialize()
	{
		ModuleManager::Get().LoadModule("Core");
		ModuleManager::Get().LoadModule("CoreSettings");
		ModuleManager::Get().LoadModule("CoreApplication");
		ModuleManager::Get().LoadModule("CoreMedia");
		ModuleManager::Get().LoadModule("CoreRHI");
#if PAL_TRAIT_VULKAN_SUPPORTED
		ModuleManager::Get().LoadModule("VulkanRHI");
#endif
		ModuleManager::Get().LoadModule("CoreRPI");
		ModuleManager::Get().LoadModule("CoreShader");

		ModuleManager::Get().LoadModule("System");
		ModuleManager::Get().LoadModule("EditorCore");
	}

	void AssetProcessor::PostInit()
	{
		auto app = PlatformApplication::Get();
		app->Initialize();

		RHI::gDynamicRHI = new CE::Vulkan::VulkanRHI();

		RHI::gDynamicRHI->Initialize();
		RHI::gDynamicRHI->PostInitialize();

		assetDefRegistry = CreateObject<AssetDefinitionRegistry>(GetTransientPackage(), "AssetDefinitionRegistry");
	}

	void AssetProcessor::PreShutdown()
	{
		auto app = PlatformApplication::Get();

		RHI::gDynamicRHI->PreShutdown();

		app->PreShutdown();
	}

	void AssetProcessor::Shutdown()
	{
		auto app = PlatformApplication::Get();

		RHI::gDynamicRHI->Shutdown();
		app->Shutdown();

		delete RHI::gDynamicRHI;
		RHI::gDynamicRHI = nullptr;

		delete app;
		app = nullptr;

		ModuleManager::Get().UnloadModule("EditorCore");
		ModuleManager::Get().UnloadModule("System");

		ModuleManager::Get().UnloadModule("CoreShader");
		ModuleManager::Get().UnloadModule("CoreRPI");
#if PAL_TRAIT_VULKAN_SUPPORTED
		ModuleManager::Get().UnloadModule("VulkanRHI");
#endif
		ModuleManager::Get().UnloadModule("CoreRHI");
		ModuleManager::Get().UnloadModule("CoreMedia");
		ModuleManager::Get().UnloadModule("CoreApplication");
		ModuleManager::Get().UnloadModule("CoreSettings");
		ModuleManager::Get().UnloadModule("Core");
	}

}
