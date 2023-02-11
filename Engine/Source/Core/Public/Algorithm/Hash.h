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
	
	CORE_API SIZE_T CalculateHash(const void* data, SIZE_T length);

    /// Default implementation doesn't have any code. Specializations do all the work.
    template<typename T>
    SIZE_T GetHash(const T& Value)
    {
        constexpr bool isPointer = std::is_pointer_v<T>;
        
        typedef std::remove_pointer_t<T> NotPtrType;
        
        if constexpr (isPointer)
        {
            return (SIZE_T)(NotPtrType*)Value;
        }
        else
        {
            // Do nothing : Specialization handles it
			return 0;
        }
    }

	template <class T>
	inline void CombineHash(SIZE_T& seed, const T& v)
	{
		seed ^= GetHash<T>(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	CE_INLINE SIZE_T GetCombinedHash(SIZE_T hashA, SIZE_T hashB)
	{
		hashA ^= (hashB + 0x9e3779b9 + (hashA << 6) + (hashA >> 2));
		return hashA;
	}

	DECL_HASH(u8)
	DECL_HASH(u16)
	DECL_HASH(u32)
	DECL_HASH(u64)

	DECL_HASH(s8)
	DECL_HASH(s16)
	DECL_HASH(s32)
	DECL_HASH(s64)

	DECL_HASH(f32)
	DECL_HASH(f64)

	DECL_HASH(bool)
	DECL_HASH(char)

	template<typename T>
	struct HashFunc
	{
		SIZE_T operator()(const T& Value) const
		{
			return CE::template GetHash(Value);
		}
	};


} // namespace CE
