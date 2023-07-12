
#include "EditorLauncher.h"

extern int GuardedMain(int argc, char** argv);

#if IS_GUI_APP
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	return GuardedMain(__argc, __argv);
}

