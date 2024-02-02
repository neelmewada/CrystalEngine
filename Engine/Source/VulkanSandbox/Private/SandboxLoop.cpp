#include "VulkanSandbox.h"

#include <cxxopts.hpp>

namespace CE::Sandbox
{
	SandboxLoop::SandboxLoop()
	{

	}

	SandboxLoop::~SandboxLoop()
	{

	}

	void SandboxLoop::PreInit(int argc, char** argv)
	{
		// Setup before loading anything

		gProjectPath = PlatformDirectories::GetLaunchDir();
		String folderName = gProjectPath.GetFilename().GetString();

		gProjectName = "Vulkan Sandbox";

		if (gDefaultWindowWidth == 0)
			gDefaultWindowWidth = 1280;
		if (gDefaultWindowHeight == 0)
			gDefaultWindowHeight = 720;

		// Initialize logging
		Logger::Initialize();
		Logger::SetConsoleLogLevel(LogLevel::Trace);
		Logger::SetFileDumpLogLevel(LogLevel::Trace);

		gProgramArguments.Clear();
		for (int i = 0; i < argc; i++)
		{
			gProgramArguments.Add(argv[i]);
		}

		cxxopts::Options options = cxxopts::Options(gProjectName.ToStdString(), (gProjectName + " command line arguments.").ToStdString());

		options.add_options()
			("h,help", "Print this help info and exit")
			;

		options.allow_unrecognised_options();

		try
		{
			cxxopts::ParseResult result = options.parse(argc, argv);

			if (result["h"].as<bool>())
			{
				std::cout << options.help() << std::endl;
				exit(0);
				return;
			}


		}
		catch (std::exception exc)
		{
			CE_LOG(Error, All, "Failed to parse arguments: {}", exc.what());
			exit(-1);
		}
	}

	void SandboxLoop::Init()
	{
		ModuleManager::Get().LoadModule("Core");
		ModuleManager::Get().LoadModule("CoreApplication");
		ModuleManager::Get().LoadModule("CoreMedia");
		ModuleManager::Get().LoadModule("CoreRHI");
		ModuleManager::Get().LoadModule("VulkanRHI");
		ModuleManager::Get().LoadModule("CoreRPI");
		ModuleManager::Get().LoadModule("CoreShader");

	}

	void SandboxLoop::PostInit()
	{
		auto app = PlatformApplication::Get();
		app->Initialize();

		gDefaultWindowWidth = 1280;
		gDefaultWindowHeight = 720;

		PlatformWindow* mainWindow = app->InitMainWindow(MODULE_NAME, gDefaultWindowWidth, gDefaultWindowHeight, false, false);

		RHI::gDynamicRHI = new Vulkan::VulkanRHI();

		RHI::gDynamicRHI->Initialize();
		RHI::gDynamicRHI->PostInitialize();

		main = new VulkanSandbox();

		main->Init(mainWindow);
	}

	void SandboxLoop::RunLoop()
	{
		auto app = PlatformApplication::Get();

		while (!IsEngineRequestingExit())
		{
			auto curTime = clock();
			f32 deltaTime = ((f32)(curTime - previousTime)) / CLOCKS_PER_SEC;

			app->Tick();

			main->Tick(deltaTime);

			previousTime = curTime;
		}
	}

	void SandboxLoop::PreShutdown()
	{
		main->Shutdown();

		auto app = PlatformApplication::Get();

		RHI::gDynamicRHI->PreShutdown();

		app->PreShutdown();
	}

	void SandboxLoop::Shutdown()
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


} // namespace CE
