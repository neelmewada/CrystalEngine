
#include "EditorCore.h"
#include "EditorSystem.h"

int GuardedMain(int argc, char** argv);

int GuardedMain(int argc, char** argv)
{
	using namespace CE::Editor;

	EditorQtApplication app{ argc, argv };

	app.Initialize();

	return app.exec();
}

