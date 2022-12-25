
#include "CoreMinimal.h"
#include "System.h"

#include <iostream>

static CE::IO::MemoryStream GStream{};

void Serialize()
{
    using namespace CE;
    
    Scene* scene = new Scene("Test Scene");
    
    CE_LOG(Info, All, "Is assignable: {}", scene->GetType()->IsAssignableTo(TYPEID(Object)));
    
    GameObject* go = new GameObject("Test GameObject");
    
    scene->AddGameObject(go);
    
    TransformComponent* transform = go->AddComponent<TransformComponent>();
    
    SerializedObject so = SerializedObject(scene);
    so.Serialize(GStream);
    
    CE_LOG(Info, All, "Serialized Data:\n{}", GStream.GetRawPointer());
    
    delete scene;
}

void Deserialize()
{
    using namespace CE;
    
    Scene* scene = new Scene("New Scene");
    
    SerializedObject so = SerializedObject(scene);
    so.Deserialize(GStream);
    GStream.Free();
    
    delete scene;
}

int main(int argc, char** argv)
{
	using namespace CE;
    
	CE::Logger::Initialize();
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("System");
    
    Serialize();
    
    Deserialize();
    
	CE::ModuleManager::Get().UnloadModule("System");
	CE::ModuleManager::Get().UnloadModule("Core");
	CE::Logger::Shutdown();
    
    return 0;
}
