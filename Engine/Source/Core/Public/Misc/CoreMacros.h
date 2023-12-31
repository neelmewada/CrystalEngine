#pragma once

#define null nullptr

// Preprocessor utilities
// Macros with __ prefix are considered implementation details and should not be used or relied upon

// Converts parameter to string, guarantees macro expansion 2 levels
// Ex: CE_TOSTRING(arg) <=> "arg"
#define CE_TOSTRING(arg)  __CE_TOSTRING1(arg)
#define __CE_TOSTRING1(arg) __CE_TOSTRING2(arg)
#define __CE_TOSTRING2(arg) #arg

// Concatenates macro parameters text to string, guarantees macro expansion 2 levels
#define CE_CONCATENATE(arg1, arg2)   __CE_CONCATENATE1(arg1, arg2)
#define __CE_CONCATENATE1(arg1, arg2)  __CE_CONCATENATE2(arg1, arg2)
#define __CE_CONCATENATE2(arg1, arg2)  arg1 ## arg2

//Used most notably for MSVC's different behavior in handling __VA_ARGS__.
//In MSVC __VA_ARGS__ is expanded as a single token which means the next macro will only have one parameter
//To work around this replace MACRO(__VA_ARGS__) by CE_MACRO_EXPAND(MACRO(__VA_ARGS)) which will perform correct substitution
//More background info in https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define CE_MACRO_EXPAND(va_args) va_args

#define CE_EXPAND(va_args) va_args

#define __CE_FIRST_ARG(First, ...) First
#define CE_FIRST_ARG(...) CE_MACRO_EXPAND(__CE_FIRST_ARG(__VA_ARGS__))

#define __CE_NON_FIRST_ARGS(First, ...) __VA_ARGS__
#define CE_NON_FIRST_ARGS(...) CE_MACRO_EXPAND(__CE_NON_FIRST_ARGS(__VA_ARGS__))

#ifdef _MSC_VER // Microsoft compilers

#   define CE_ARG_COUNT(...)  __CE_EXPAND_ARGS_PRIVATE(__CE_ARGS_AUGMENTER(__VA_ARGS__))

#   define __CE_ARGS_AUGMENTER(...) unused, __VA_ARGS__
#   define __CE_EXPAND(x) x
#   define __CE_EXPAND_ARGS_PRIVATE(...) __CE_EXPAND(__CE_GET_ARG_COUNT_PRIVATE(__VA_ARGS__, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#   define __CE_GET_ARG_COUNT_PRIVATE(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count

#else // Non-Microsoft compilers

#   define CE_ARG_COUNT(...) __CE_GET_ARG_COUNT_PRIVATE(0, ## __VA_ARGS__, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#   define __CE_GET_ARG_COUNT_PRIVATE(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count

#endif

#if PAL_TRAIT_BUILD_STANDALONE
#define ENGINE_CONST const
#else
#define ENGINE_CONST
#endif

namespace CE::Internal
{

    template<typename T, SIZE_T N>
    constexpr inline SIZE_T CountOf(const T (&arr)[N])
    {
        return N;
    }
}

#define COUNTOF(x) CE::Internal::CountOf(x)

