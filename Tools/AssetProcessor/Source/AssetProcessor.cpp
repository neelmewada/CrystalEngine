#include "AssetProcessor.h"

#define LOG(x) std::cout << x << std::endl;

AssetProcessor::AssetProcessor(int argc, char** argv)
{
	options.add_options()
		("h,help", "Print this help info.")
		("p,pack", "Package the assets for final distribution build. (Not implemented yet)")
		("I,include", "Include path directories for shaders, etc.", cxxopts::value<std::vector<std::string>>())
		("M,mode", "Processing mode: either Engine or Game. If assets should be processed as engine or game assets", cxxopts::value<std::string>()->default_value("Game"))
		("i,input", "Paths to the input assets that is to be processed.", cxxopts::value<std::vector<std::string>>())
		("R,root", "Path to root of the assets directory.", cxxopts::value<std::string>())
		("A,all", "Process all assets, including the one that are already processed.")
		("N,new", "Only process assets that haven't been processed.")
		;

	try
	{
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

	String mode = parsedOptions["M"].as<std::string>();
	if (mode == "Game")
	{
		this->mode = AssetMode::Game;
	}
	else if (mode == "Engine")
	{
		this->mode = AssetMode::Engine;
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

	if (parsedOptions["A"].as<bool>())
	{
		processMode = ProcessMode::All;
	}
	else if (parsedOptions["N"].as<bool>())
	{
		processMode = ProcessMode::New;
	}
	else if (parsedOptions.count("i") > 0)
	{
		processMode = ProcessMode::Individual;
		
		auto inputs = parsedOptions["i"].as<std::vector<std::string>>();
		
		for (const auto& input : inputs)
		{
			individualAssetPaths.Add(input);
		}
	}

	assetsDir = parsedOptions["R"].as<std::string>();
	assetsDir = assetsDir.GetString().Replace({ '\\' }, '/');

	gProjectPath = assetsDir.GetParentPath().GetParentPath();
	gProjectName = "AssetProcessor";

	assetsDir.RecursivelyIterateChildren([&](const IO::Path& path)
		{
			auto extension = path.GetExtension();
			if (!path.IsDirectory() && extension != ".casset")
			{
				auto relative = IO::Path::GetRelative(path, assetsDir).GetString().Replace({ '\\' }, '/');
				allSourceAssetPaths.Add(path);
			}
		});
	
	allSourceAssetPaths.GetSize();
}

AssetProcessor::~AssetProcessor()
{
}

int AssetProcessor::Run()
{
	Initialize();
	PostInit();
	
	JobContext* jobContext = JobContext::GetGlobalContext();
	JobManager* jobManager = jobContext->GetJobManager();

	HashMap<AssetDefinition*, Array<IO::Path>> sourcePathsByAssetDef{};

	for (const auto& sourcePath : allSourceAssetPaths)
	{
		if (!sourcePath.Exists())
			continue;

		String extension = sourcePath.GetExtension().GetString();

		AssetDefinition* assetDef = assetDefRegistry->FindAssetDefinition(extension);

		if (assetDef != nullptr)
		{
			sourcePathsByAssetDef[assetDef].Add(sourcePath);
		}
	}

	Array<AssetImporter*> importers{};

	for (const auto& [assetDef, sourcePaths] : sourcePathsByAssetDef)
	{
		if (assetDef == nullptr || sourcePaths.IsEmpty())
			continue;

		ClassType* assetImporterClass = assetDef->GetAssetImporterClass();
		if (assetImporterClass == nullptr || !assetImporterClass->CanBeInstantiated())
			continue;
		auto assetImporter = CreateObject<AssetImporter>(nullptr, "AssetImporter", OF_Transient, assetImporterClass);
		if (assetImporter == nullptr)
			continue;

		assetImporter->SetIncludePaths(includePaths);
		assetImporter->SetLogging(true);

		importers.Add(assetImporter);
		assetImporter->ImportSourceAssetsAsync(sourcePaths);
	}

	// Wait for all jobs to complete
	jobManager->Complete();

	PreShutdown();
	Shutdown();

	return 0;
}

void AssetProcessor::Initialize()
{
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("CoreShader");

	
}

void AssetProcessor::PostInit()
{
	auto app = PlatformApplication::Get();
	app->Initialize();

	gDefaultWindowWidth = 1280;
	gDefaultWindowHeight = 720;

	PlatformWindow* mainWindow = app->InitMainWindow(MODULE_NAME, gDefaultWindowWidth, gDefaultWindowHeight, false, false);

	RHI::gDynamicRHI = new CE::Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();

	assetDefRegistry = CreateObject<AssetDefinitionRegistry>(GetTransientPackage(), "AssetDefinitionRegistry");

	Logger::Initialize();
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

	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}
