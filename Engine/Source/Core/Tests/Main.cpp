
#include "CoreMinimal.h"

#include <iostream>
#include <any>

#define LOG(x) std::cout << x << std::endl



int main(int argc, char* argv[])
{
    using namespace CE;

    CE::Logger::Initialize();
    CE::ModuleManager::Get().LoadModule("Core");
    
    

    CE::ModuleManager::Get().UnloadModule("Core");
    CE::Logger::Shutdown();
    return 0;
}

