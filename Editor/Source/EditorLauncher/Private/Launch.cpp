
#include "CoreMinimal.h"

#include "EditorCore.h"
#include "EditorSystem.h"
#include "CrystalEditor.h"

static CE::Editor::EditorLoop GEditorLoop{};

int GuardedMain(int argc, char** argv);

int GuardedMain(int argc, char** argv)
{
	return GEditorLoop.RunLoop(argc, argv);
}

