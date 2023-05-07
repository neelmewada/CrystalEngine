
#include "cxxopts.hpp"

#include "CoreMinimal.h"

#include "RTTI/RTTIGenerator.h"

using namespace CE;

int main(int argc, char** argv)
{
    using namespace CE;

    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    CE_REGISTER_TYPES(HeaderCRC, ModuleStamp);

    cxxopts::Options options("Auto RTTI Tool", "A command line tool to auto generate RTTI information for modules");

    options.add_options()
        ("h,help", "Print this help info.")
        ("m,module", "Module Name", cxxopts::value<std::string>())
        ("d,dir", "Module root path", cxxopts::value<std::string>())
        ("o,output", "Generated file output path", cxxopts::value<std::string>())
        ("I,inc", "Include search directories", cxxopts::value<std::vector<std::string>>()->default_value(""))
        ("f,force", "Force update output headers")
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

        if (!outPath.Exists())
        {
            IO::Path::CreateDirectories(outPath);
        }

        if (!modulePath.Exists())
        {
            CE_LOG(Error, All, "Failed to generate RTTI for module: {}\nGiven module path doesn't exist: {}", moduleName, modulePath);
            return 1;
        }

        ModuleStamp moduleStamp{};
        IO::Path moduleStampPath = outPath / (moduleName + ".stamp");

        bool forceUpdate = result["f"].as<bool>();
        
        if (!forceUpdate)
        {
            outPath.IterateChildren([&](const IO::Path& child){
                if (child.GetExtension() == ".stamp")
                {
                    FileStream fileStream{child, Stream::Permissions::ReadOnly};
                    fileStream.SetBinaryMode(true);
                    if (fileStream.IsOpen())
                    {
                        String fileName = "";
                        fileStream >> fileName;
                        u32 crc = 0;
                        fileStream >> crc;
                        auto fullFilePath = child.GetParentPath() / fileName;
                        if (fullFilePath.Exists() && crc != 0)
                            moduleStamp.headers.Add(HeaderCRC(fullFilePath, crc));
                        fileStream.Close();
                    }
                }
            });
        }
        

        RTTIGenerator::GenerateRTTI(moduleName, modulePath, outPath, moduleStamp);

        for (const HeaderCRC& header : moduleStamp.headers)
        {
            if (!header.headerPath.Exists())
                continue;
            IO::Path stampFilePath = header.headerPath.GetString() + ".stamp";
            if (stampFilePath.Exists())
                IO::Path::Remove(stampFilePath);
            FileStream fileStream{stampFilePath, Stream::Permissions::WriteOnly};
            if (fileStream.IsOpen())
            {
                String fileName = header.headerPath.GetFilename().GetString();
                fileStream << fileName;
                fileStream << header.crc;
                fileStream.Close();
            }
        }

        CE_LOG(Info, All, "Automatic RTTI for module: {}", moduleName);
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
