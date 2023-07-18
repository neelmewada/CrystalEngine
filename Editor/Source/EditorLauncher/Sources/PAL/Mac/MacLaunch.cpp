
#include "EditorLauncher.h"

extern int GuardedMain(int argc, char** argv);

int main(int argc, char** argv)
{
	return GuardedMain(argc, argv);
}

