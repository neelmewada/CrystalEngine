#pragma once

#include <stdint.h>

namespace CE
{
    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef int8_t  s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef float  f32;
    typedef double f64;

    typedef char c8;
    
    typedef intptr_t IntPtr;
    typedef uintptr_t UintPtr;

    typedef ptrdiff_t PtrDiff;
}

typedef char TCHAR;
typedef wchar_t WCHAR;

#if IS_64BIT
    typedef CE::u64 SIZE_T;
    typedef CE::s64 SSIZE_T;
#else
    typedef CE::u32 SIZE_T;
    typedef CE::s32 SSIZE_T;
#endif

