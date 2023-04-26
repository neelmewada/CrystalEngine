
#include "CoreMinimal.h"
#include "System.h"

#include <iostream>


int main(int argc, char** argv)
{
	using namespace CE;
    
	CE::Logger::Initialize();
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("CoreMedia");
	CE::ModuleManager::Get().LoadModule("System");

	Scene* scene = new Scene("Test Scene");

	GameObject* go = new GameObject(scene);

	

	SerializedObject so{ scene };
	IO::MemoryStream memStream{};
	so.Serialize(memStream);

	std::cout << memStream.GetBuffer() << std::endl;

	memStream.Free();

	delete go;
	delete scene;

	CE::ModuleManager::Get().UnloadModule("System");
	CE::ModuleManager::Get().UnloadModule("CoreMedia");
	CE::ModuleManager::Get().UnloadModule("Core");
	CE::Logger::Shutdown();
    
    return 0;
}
