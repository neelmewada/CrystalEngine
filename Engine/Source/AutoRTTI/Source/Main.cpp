
#include "cxxopts.hpp"

#include "CoreMinimal.h"

#include "RTTI/RTTIGenerator.h"

int main(int argc, char** argv)
{
    using namespace CE;

    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    cxxopts::Options options("Auto RTTI Tool", "A command line tool to auto generate RTTI information for modules");

    options.add_options()
        ("h,help", "Print this help info.")
        ("m,module", "Module Name", cxxopts::value<std::string>())
        ("d,dir", "Module root path", cxxopts::value<std::string>())
        ("o,output", "Generated file output path", cxxopts::value<std::string>())
        ("I,inc", "Include search directories", cxxopts::value<std::vector<std::string>>())
        ;

    try
    {
        cxxopts::ParseResult result = options.parse(argc, argv);

        if (result["h"].as<bool>())
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        String moduleName = result["m"].as<std::string>();
        IO::Path modulePath = result["d"].as<std::string>();
        IO::Path outPath = result["o"].as<std::string>();
        auto searchDirs = result["I"].as<std::vector<std::string>>();

        if (!outPath.Exists())
        {
            IO::Path::CreateDirectories(outPath);
        }

        if (!modulePath.Exists())
        {
            CE_LOG(Error, All, "Failed to generate RTTI for module: {}\nGiven module path doesn't exist: {}", moduleName, modulePath);
            return 1;
        }

        RTTIGenerator::includeSearchPaths.Clear();
        for (auto dir : searchDirs)
        {
            RTTIGenerator::includeSearchPaths.Add(dir);
        }

        RTTIGenerator::GenerateRTTI(moduleName, modulePath, outPath);

        CE_LOG(Info, All, "Generated RTTI for module: {}", moduleName);
    }
    catch (std::exception exc)
    {
        CE_LOG(Error, All, "Failed to parse input arguments: {}", exc.what());
        return -1;
    }

    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");

    return 0;
}
