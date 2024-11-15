#pragma once


constexpr SIZE_T operator"" _KB(unsigned long long Value)
{
    return Value * 1024;
}

constexpr SIZE_T operator"" _MB(unsigned long long Value)
{
    return Value * (1024 * 1024);
}

constexpr SIZE_T operator"" _GB(unsigned long long Value)
{
    return Value * (1024 * 1024 * 1024);
}

constexpr SIZE_T operator"" _Bytes(unsigned long long Value)
{
    return Value;
}
