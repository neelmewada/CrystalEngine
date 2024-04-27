#pragma once

#define CE_MAKE_VERSION(major, minor, patch, build) (u64)build | ((u64)patch << 16) | ((u64)minor << (16*2)) | ((u64)major << (16*3))

#define CE_ENGINE_VERSION CE_MAKE_VERSION(CE_VERSION_MAJOR, CE_VERSION_MINOR, CE_VERSION_PATCH, CE_VERSION_BUILD)

#define CE_ENGINE_VERSION_STRING_SHORT CE_TOSTRING(CE_VERSION_MAJOR) "." CE_TOSTRING(CE_VERSION_MINOR) "." CE_TOSTRING(CE_VERSION_PATCH)

#define CE_ENGINE_VERSION_STRING CE_TOSTRING(CE_VERSION_MAJOR) "." CE_TOSTRING(CE_VERSION_MINOR) "." CE_TOSTRING(CE_VERSION_PATCH) "." CE_TOSTRING(CE_VERSION_BUILD)

#define CE_ENGINE_NAME_STRING "Crystal Engine"

#define CE_DEPRECATED(Version, Message) [[deprecated(Message " Please update your code to not use the deprecated API.")]]

#define CE_NO_COPY(Class) Class(const Class&) = delete; Class& operator=(const Class&) = delete;

#define CE_NO_MOVE(Class) Class(Class&&) = delete; Class& operator=(Class&&) = delete;

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

#define TEXT(x) x

#define NAMEOF(x) (CE::GetStaticType<x>() != nullptr ? CE::GetStaticType<x>()->GetName() : "")

#define CE_INLINE inline
#define INLINE inline

#if PLATFORM_WINDOWS
#	define CE_FORCE_INLINE __forceinline
#	define FORCE_INLINE __forceinline
#else
#	define CE_FORCE_INLINE inline
#	define FORCE_INLINE inline
#endif

#define CE_DELETE(ptr) delete ptr; ptr = nullptr

#ifndef NULL
#define NULL 0
#endif

#define StaticCast static_cast

