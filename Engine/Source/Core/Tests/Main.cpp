
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl



int main(int argc, char* argv[])
{
    using namespace CE;

    CE::ModuleManager::Get().LoadModule("Core");
    CE::Logger::Initialize();

    Archive archive{};
    archive.Open(PlatformDirectories::GetLaunchDir() / "plugins.zip", ArchiveMode::Read);

    auto totalEntries = archive.GetTotalEntries();

    for (int i = 0; i < totalEntries; i++)
    {
        archive.OpenEntry(i);
        String name = archive.EntryReadName();
        LOG("Entry " << i << ": Name = " << name.GetCString() << " | Size = " << archive.EntryReadSize());
        archive.CloseEntry();
    }

    archive.Close();
    
    CE::Logger::Shutdown();
    CE::ModuleManager::Get().UnloadModule("Core");
    return 0;
}

