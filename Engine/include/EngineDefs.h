#pragma once

#if PLATFORM_WIN32
#   ifdef ENGINE_DLL_EXPORT
#       define ENGINE_API __declspec(dllexport)
#   else
#       define ENGINE_API __declspec(dllimport)
#   endif
#else
#   define ENGINE_API
#endif

