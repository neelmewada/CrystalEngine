#pragma once

#include "Types/CoreTypeDefs.h"

#include <functional>

#define DECL_HASH(Type)\
template<>\
inline SIZE_T GetHash<Type>(const Type& Value)\
{\
	return std::hash<Type>()(Value);\
}

namespace CE
{

	template<typename T>
	SIZE_T GetHash(const T& Value);

	CORE_API SIZE_T CalculateHash(const void* data, SIZE_T length);

	template <class T>
	inline void CombineHash(SIZE_T& seed, const T& v)
	{
		seed ^= GetHash<T>(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	DECL_HASH(u8)
	DECL_HASH(u16)
	DECL_HASH(u32)
	DECL_HASH(u64)

	DECL_HASH(s8)
	DECL_HASH(s16)
	DECL_HASH(s32)
	DECL_HASH(s64)


	template<typename T>
	struct HashFunc
	{
		SIZE_T operator()(const T& Value) const
		{
			return GetHash(Value);
		}
	};


} // namespace CE
