
#include "CoreMinimal.h"
#include "System.h"

#include <iostream>


int main(int argc, char** argv)
{
	using namespace CE;
    
	CE::Logger::Initialize();
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("System");
    
	CE::ModuleManager::Get().UnloadModule("System");
	CE::ModuleManager::Get().UnloadModule("Core");
	CE::Logger::Shutdown();
    
    return 0;
}
