#pragma once

#define FROM_BIG_ENDIAN CE::FromBigEndian

#define FROM_LITTLE_ENDIAN CE::FromLittleEndian


namespace CE
{

	CORE_API void SwapByteOrder(void* value, u32 length);

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void FromBigEndian(TInt& valueRef)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&valueRef, sizeof(TInt));
#endif
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void FromLittleEndian(TInt& valueRef)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&valueRef, sizeof(TInt));
#endif
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt FromBigEndian(TInt value)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt FromLittleEndian(TInt value)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt ToBigEndian(TInt value)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void ToBigEndian(TInt& value)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt ToLittleEndian(TInt value)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void ToLittleEndian(TInt& value)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
	}


} // namespace CE

