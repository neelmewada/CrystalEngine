#pragma once

#define CE_MAKE_VERSION(major, minor, patch, build) ((u64)major << (16*16*16)) | ((u64)minor << (16*16)) | ((u64)patch << 16) | (u64)build

#define CE_ENGINE_VERSION CE_MAKE_VERSION(0, 1, 0, 0)
#define CE_ENGINE_VERSION_STRING "0.1.0.0"

#define CE_DEPRECATED(Version, Message) [[deprecated(Message " Please update your code to not use the deprecated API.")]]

#define CE_NO_COPY(Class) Class(Class& Copy) = delete;

#define CE_STATIC_CLASS(Class) Class() = delete; ~Class() = delete;

#define CE_SINGLETON_CLASS(Class) Class(Class& Copy) = delete; Class operator=(Class& copy) = delete;

#if PLATFORM_WINDOWS
#	define DLL_EXPORT __declspec(dllexport)
#	define DLL_IMPORT __declspec(dllimport)
#else
#	define DLL_EXPORT 
#	define DLL_IMPORT 
#endif

#define BIT(x) (1 << x)

#define TEXT(x) #x

#define NAME(x) CE::Name(#x)

#define CE_INLINE inline

#define CE_FORCE_INLINE __forceinline

#define CE_DELETE(ptr) delete ptr; ptr = nullptr
