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
		this->mode = AssetMode::Game;
	else if (mode == "Engine")
		this->mode = AssetMode::Engine;
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

	assetsDir.RecursivelyIterateChildren([&](const IO::Path& path)
		{
			auto extension = path.GetExtension();
			if (!path.IsDirectory() && extension != ".casset")
			{
				auto relative = IO::Path::GetRelative(path, assetsDir).GetString().Replace({ '\\' }, '/');
				allSourceAssetPaths.Add(relative);
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
	
	JobContext* jobContext = JobContext::GetGlobalContext();
	JobManager* jobManager = jobContext->GetJobManager();

	for (const auto& sourcePath : allSourceAssetPaths)
	{
		if (sourcePath.GetExtension() == ".shader")
		{
			FileStream stream = FileStream(assetsDir / sourcePath, Stream::Permissions::ReadOnly);
			if (!stream.IsOpen())
			{
				continue;
			}

			ShaderPreprocessor preprocessor{ &stream, includePaths };

			auto preprocessData = preprocessor.PreprocessShader();
		}
	}

	Shutdown();

	return 0;
}

void AssetProcessor::Initialize()
{
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	
	ModuleManager::Get().LoadModule("System");
	ModuleManager::Get().LoadModule("EditorCore");

	assetDefRegistry = CreateObject<AssetDefinitionRegistry>(GetTransientPackage(), "AssetDefinitionRegistry");

	Logger::Initialize();
}

void AssetProcessor::Shutdown()
{
	Logger::Shutdown();

	assetDefRegistry->Destroy();
	assetDefRegistry = nullptr;

	ModuleManager::Get().UnloadModule("EditorCore");
	ModuleManager::Get().UnloadModule("System");

	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("Core");
}
