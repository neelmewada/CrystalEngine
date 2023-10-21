
int GuardedMain(int argc, char** argv);

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if CE_BUILD_RELEASE
int WINAPI WinMain(HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
	HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
	LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
	int iCmdShow)
#else
int main(int argc, char** argv)
#endif
{
    return GuardedMain(__argc, __argv);
}

