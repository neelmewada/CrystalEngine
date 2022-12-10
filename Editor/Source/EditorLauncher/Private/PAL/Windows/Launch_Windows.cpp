
#include <Windows.h>

int GuardedMain(int argc, char** argv);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return GuardedMain(__argc, __argv);
}
