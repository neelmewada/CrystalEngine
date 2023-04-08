
#include <Windows.h>

int GuardedMain(int argc, char** argv);

#if CE_BUILD_RELEASE
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main(int argc, char** argv)
#endif
{
	return GuardedMain(__argc, __argv);
}
