#include "AssetProcessor.h"

#define LOG(x) std::cout << x << std::endl;
#define LOGERR(x) std::cerr << x << std::endl;

namespace CE
{
	constexpr u32 StampFileVersion = 0;

	AssetProcessor::AssetProcessor(int argc, char** argv)
	{
		options.add_options()
			("h,help", "Print this help info.")
			("p,pack", "Bundle the assets for final distribution build. (Not implemented yet)")
			("I,include", "Include path directories for shaders, etc.", cxxopts::value<std::vector<std::string>>())
			("M,mode", "Processing mode: either Engine or Game. If assets should be processed as engine or game assets.", cxxopts::value<std::string>()->default_value("Game"))
			("R,output-root", "Path to root of the assets directory.", cxxopts::value<std::string>())
			("D,input-root", "Path to root of the assets directory.", cxxopts::value<std::string>())
			("T,target", "Target platform. Values: Windows, Linux, Mac, Android, iOS", cxxopts::value<std::string>()->default_value(""))
			("t,temp", "Temporary directory path", cxxopts::value<std::string>())
			("P,project", "Project root directory", cxxopts::value<std::string>()->default_value(""))
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
			LOGERR("Failed to parse arguments: " << exc.what());
			exit(-1);
		}

		tempDir = parsedOptions["t"].as<std::string>();
		inputRoot = parsedOptions["D"].as<std::string>();
		outputRoot = parsedOptions["R"].as<std::string>();

		tempDir = tempDir / "AssetCache";

		String targetName = parsedOptions["T"].as<std::string>();
		String projectPath = parsedOptions["P"].as<std::string>();

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
			tempDir = tempDir / "Game";
		}
		else if (mode == "Engine")
		{
			this->mode = AssetMode::Engine;
			tempDir = tempDir / "Engine";
		}
		else if (mode == "Editor")
		{
			this->mode = AssetMode::Editor;
			tempDir = tempDir / "Editor";
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
	
		auto inputs = parsedOptions.unmatched();
		
		for (const auto& input : inputs)
		{
			individualAssetPaths.Add(input);
		}

		gProjectPath = PlatformDirectories::GetLaunchDir();
		if (projectPath.NonEmpty())
			gProjectPath = projectPath;
		gProjectName = "AssetProcessor";
	
		allSourceAssetPaths.Clear();
		allProductAssetPaths.Clear();

		if (individualAssetPaths.NonEmpty())
		{
			allSourceAssetPaths.AddRange(individualAssetPaths);
		}
		else // process only modified files
		{
			inputRoot.RecursivelyIterateChildren([&](const IO::Path& path)
				{
					if (path.IsDirectory())
						return;
					String fileName = path.GetFilename().GetString();
					String extension = path.GetExtension().GetString();
					if (extension == ".cmake" || fileName == "CMakeLists.txt")
						return;

					DateTime lastWriteTime = path.GetLastWriteTime();

					IO::Path relativePath = IO::Path::GetRelative(path, inputRoot);
					IO::Path stampFilePath = tempDir / relativePath.ReplaceExtension(".stamp");

					if (!stampFilePath.GetParentPath().Exists())
					{
						IO::Path::CreateDirectories(stampFilePath.GetParentPath());
					}

					IO::Path productPath{};

					if (inputRoot != outputRoot)
					{
						IO::Path relativeSourcePath = IO::Path::GetRelative(path, inputRoot);
						productPath = outputRoot / relativeSourcePath;
						productPath = productPath.ReplaceExtension(".casset");
						if (!productPath.GetParentPath().Exists())
						{
							IO::Path::CreateDirectories(productPath.GetParentPath());
						}
					}
					else
					{
						productPath = path.ReplaceExtension(".casset");
					}

					if (!stampFilePath.Exists() || !productPath.Exists())
					{
						allSourceAssetPaths.Add(path);
						allProductAssetPaths.Add(productPath);
					}
					else // File exists
					{
						FileStream reader = FileStream(stampFilePath, Stream::Permissions::ReadOnly);
						reader.SetBinaryMode(true);
						
						u32 version = 0;
						reader >> version;
						
						u64 stampedTimeInt = 0;
						reader >> stampedTimeInt;

						DateTime stampedTime = DateTime::FromNumber(stampedTimeInt);

						if (lastWriteTime != stampedTime) // Source asset modified
						{
							allSourceAssetPaths.Add(path);
							allProductAssetPaths.Add(productPath);
						}
					}
				});
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

		// Paths to product assets to be generated in this run
		HashSet<IO::Path> productAssetPaths{};

		for (int i = 0; i < allSourceAssetPaths.GetSize(); i++)
		{
			auto sourcePath = allSourceAssetPaths[i];
			auto productPath = allProductAssetPaths[i];
			sourcePath = sourcePath.GetString().Replace({ '\\' }, '/');
			productPath = productPath.GetString().Replace({ '\\' }, '/');

			if (!sourcePath.Exists())
				continue;

			String extension = sourcePath.GetExtension().GetString().ToLower();

			AssetDefinition* assetDef = assetDefRegistry->FindAssetDefinition(extension);

			if (assetDef != nullptr)
			{
				sourcePathsByAssetDef[assetDef].Add(sourcePath);
				productPathsByAssetDef[assetDef].Add(productPath);
				productAssetPaths.Add(productPath);
			}
		}

		Array<AssetImporter*> importers{};

		struct ImportJobEntry
		{
			Array<IO::Path> productAssetDependencies{};
			AssetImportJob* job = nullptr;
		};

		Array<ImportJobEntry> importJobs{};
		HashMap<IO::Path, AssetImportJob*> importJobsByProductPath{};

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

			// Get product asset dependencies required by the asset importer.

			Array<Name> productAssetDependencies = assetImporter->GetProductAssetDependencies();
			Array<IO::Path> productDependencies{};
			for (const Name& productAssetName : productAssetDependencies)
			{
				IO::Path assetPath = Bundle::GetBundlePath(productAssetName);
				if (productAssetPaths.Exists(assetPath))
				{
					productDependencies.Add(assetPath);
				}
				else if (assetPath.Exists()) // Nothing needed to be done
				{
					
				}
				else
				{
					CE_LOG(Warn, All, "Could not process assets of type {}. Product asset dependency not found: {}.", 
						assetImporterClass->GetName(), productAssetName);
					continue;
				}
			}

			assetImporter->SetIncludePaths(includePaths);
			assetImporter->SetLogging(true);
			assetImporter->SetTargetPlatform(targetPlatform);
			assetImporter->SetTempDirectoryPath(tempDir);

			importers.Add(assetImporter);
			
			Array<AssetImportJob*> jobs = assetImporter->PrepareImportJobs(sourcePaths, productPathsByAssetDef[assetDef]);

			for (AssetImportJob* job : jobs)
			{
				job->SetAutoDelete(true);
				importJobsByProductPath[job->GetProductPath()] = job;

				importJobs.Add({ productDependencies, job });
			}
		}

		// Setup job dependencies
		for (const ImportJobEntry& entry : importJobs)
		{
			for (const IO::Path& productDependency : entry.productAssetDependencies)
			{
				if (importJobsByProductPath[productDependency] != nullptr)
				{
					importJobsByProductPath[productDependency]->SetDependent(entry.job);
				}
			}
		}

		// Launch the jobs
		for (const ImportJobEntry& entry : importJobs)
		{
			entry.job->Start();
		}

		// Wait for all jobs to complete
		jobManager->Complete();
		Array<AssetImportJobResult> assetImportResults{};

		for (auto importer : importers)
		{
			assetImportResults.AddRange(importer->GetResults());
			importer->Destroy();
		}
		importers.Clear();

		int failCounter = 0;

		// Update time stamps
		{
			for (const auto& result : assetImportResults)
			{
				IO::Path relativePath = IO::Path::GetRelative(result.sourcePath, inputRoot);
				IO::Path stampFilePath = tempDir / relativePath.ReplaceExtension(".stamp");
				if (result.success)
				{
					FileStream writer = FileStream(stampFilePath, Stream::Permissions::WriteOnly);
					writer.SetBinaryMode(true);
					
					writer << StampFileVersion;
					writer << result.sourcePath.GetLastWriteTime().ToNumber();
				}
				else
				{
					failCounter++;
				}
			}
		}

		Logger::Shutdown();

		PreShutdown();
		Shutdown();

		return failCounter;
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
		ModuleManager::Get().LoadModule("CrystalWidgets");

		ModuleManager::Get().LoadModule("System");
		ModuleManager::Get().LoadModule("EditorCore");
		ModuleManager::Get().LoadModule("EditorSystem");
	}

	void AssetProcessor::PostInit()
	{
		auto app = PlatformApplication::Get();
		app->Initialize();

		gEngine->PreInit();

		RHI::gDynamicRHI = new CE::Vulkan::VulkanRHI();

		RHI::gDynamicRHI->Initialize();
		RHI::gDynamicRHI->PostInitialize();

		gEngine->Initialize();
		
		gEngine->PostInitialize();

		assetDefRegistry = Editor::GetAssetDefinitionRegistry();

		RPISystem::Get().Initialize();
	}

	void AssetProcessor::PreShutdown()
	{
		auto app = PlatformApplication::Get();

		RPISystem::Get().Shutdown();

		gEngine->PreShutdown();

		RHI::gDynamicRHI->PreShutdown();

		app->PreShutdown();
	}

	void AssetProcessor::Shutdown()
	{
		auto app = PlatformApplication::Get();

		gEngine->Shutdown();

		RHI::gDynamicRHI->Shutdown();
		app->Shutdown();

		delete RHI::gDynamicRHI;
		RHI::gDynamicRHI = nullptr;

		delete app;
		app = nullptr;

		ModuleManager::Get().UnloadModule("EditorSystem");
		ModuleManager::Get().UnloadModule("EditorCore");
		ModuleManager::Get().UnloadModule("System");

		ModuleManager::Get().UnloadModule("CrystalWidgets");
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
