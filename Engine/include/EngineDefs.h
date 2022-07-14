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

#ifndef _countof
#define _countof(arr) sizeof(arr) / sizeof(arr[0])
#endif

#define DELETE_COPY_CONSTRUCTORS(Class) Class(const Class&) = delete; Class operator=(const Class&) = delete;

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS

#include <stdint.h>
typedef uint64_t Uint64;
typedef uint32_t Uint32;
typedef uint16_t Uint16;
typedef uint8_t Uint8;

typedef int64_t Sint64;
typedef int32_t Sint32;
typedef int16_t Sint16;
typedef int8_t Sint8;

#include <stdexcept>

#define VOX_ASSERT(condition,message) if (!(condition)) throw std::runtime_error(message)

#define MAKE_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29) | (((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

#define MAX_MAX_SIMULTANEOUS_FRAME_DRAWS 2

#ifdef ENGINE_INTERNAL
#include "EngineInternal.h"
#endif
