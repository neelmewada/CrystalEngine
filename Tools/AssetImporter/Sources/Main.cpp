
#include "AssetProcessor.h"

#include "cxxopts.hpp"

using namespace CE;
using namespace CE::Editor;

int main(int argc, char** argv)
{
    CE::Logger::Initialize();
    CE::ModuleManager::Get().LoadModule("Core");
    CE::ModuleManager::Get().LoadModule("System");
    CE::ModuleManager::Get().LoadModule("QtComponents");
    CE::ModuleManager::Get().LoadModule("EditorCore");
    
    cxxopts::Options options("Asset Processor", "A command line and GUI tool to process Assets");

    options.add_options()
        ("h,help", "Print this help info.")
        ;

    s32 retVal = 0;

    try
    {
        cxxopts::ParseResult result = options.parse(argc, argv);

        if (result["h"].as<bool>())
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        AssetProcessorApplication app{ argc, argv };

        AssetImporterWindow window{};
        window.show();

        retVal = app.exec();
    }
    catch (std::exception exc)
    {
        CE_LOG(Error, All, "Failed to parse input arguments: {}", exc.what());
        retVal = 1;
    }

    
    CE::ModuleManager::Get().UnloadModule("EditorCore");
    CE::ModuleManager::Get().UnloadModule("QtComponents");
    CE::ModuleManager::Get().UnloadModule("System");
    CE::ModuleManager::Get().UnloadModule("Core");
    CE::Logger::Shutdown();

    return retVal;
}
