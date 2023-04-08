#pragma once


inline SIZE_T operator"" _KB(SIZE_T Value)
{
    return Value * 1024;
}

inline SIZE_T operator"" _MB(SIZE_T Value)
{
    return Value * (1024 * 1024);
}

inline SIZE_T operator"" _Bytes(SIZE_T Value)
{
    return Value;
}
