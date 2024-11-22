
#include "PAL/Windows/WindowsMisc.h"

#include <Windows.h>

#include <VersionHelpers.h>

namespace CE
{
    static thread_local String osVersionString = "";

    String WindowsMisc::GetOSVersionString()
    {
        if (osVersionString.NotEmpty())
            return osVersionString;

        NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);

        OSVERSIONINFOEXW osInfo;

        *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

        if (NULL != RtlGetVersion)
        {
            osInfo.dwOSVersionInfoSize = sizeof(osInfo);
            RtlGetVersion(&osInfo);
            
            bool isWindows11 = osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion >= 21996;
            String majorVersion = "11";
            if (!isWindows11)
                majorVersion = String::Format("{}", osInfo.dwBuildNumber);

            return String::Format("Windows {} [{}.{}.{}] (x86_64)", 
                majorVersion,
                osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber
            );
        }

        return "";
    }

    
} // namespace CE

