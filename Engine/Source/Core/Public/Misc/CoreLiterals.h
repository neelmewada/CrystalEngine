#pragma once


constexpr SIZE_T operator"" _KB(SIZE_T Value)
{
    return Value * 1024;
}

constexpr SIZE_T operator"" _MB(SIZE_T Value)
{
    return Value * (1024 * 1024);
}

constexpr SIZE_T operator"" _GB(u64 Value)
{
    return Value * (1024 * 1024 * 1024);
}

constexpr SIZE_T operator"" _Bytes(SIZE_T Value)
{
    return Value;
}
