
#include "CoreMinimal.h"

#include "EditorCore.h"
#include "EditorSystem.h"
#include "CrystalEditor.h"

int GuardedMain(int argc, char** argv);

int GuardedMain(int argc, char** argv)
{
	using namespace CE::Editor;

	CE::ModuleManager::Get().LoadModule("Core");
	CE::Logger::Initialize();

	CE::ModuleManager::Get().LoadModule("System");
	CE::ModuleManager::Get().LoadModule("EditorCore");
	CE::ModuleManager::Get().LoadModule("EditorSystem");

	CrystalEditorApplication app{ argc, argv };

	app.Initialize();

	auto ret = app.exec();

	CE::ModuleManager::Get().UnloadModule("EditorSystem");
	CE::ModuleManager::Get().UnloadModule("EditorCore");
	CE::ModuleManager::Get().UnloadModule("System");

	CE::Logger::Shutdown();
	CE::ModuleManager::Get().UnloadModule("Core");

	return ret;
}

