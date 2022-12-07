#pragma once

#define CE_DEPRECATED(Version, Message) [[deprecated(Message " Please update your code to not use the deprecated API.")]]

#define CE_NO_COPY(Class) Class(Class& Copy) = delete

#define CE_STATIC_CLASS(Class) Class() = delete; ~Class() = delete

#define CE_SINGLETON_CLASS(Class) Class(Class& Copy) = delete; Class operator=(Class& copy) = delete

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

