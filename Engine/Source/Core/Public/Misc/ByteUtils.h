#pragma once

#define FROM_BIG_ENDIAN CE::BigEndianToCurrent

#define FROM_LITTLE_ENDIAN CE::LittleEndianToCurrent

namespace CE
{

	CORE_API void SwapByteOrder(void* value, u32 length);

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void BigEndianToCurrent(TInt& valueRef)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&valueRef, sizeof(TInt));
#endif
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static void LittleEndianToCurrent(TInt& valueRef)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&valueRef, sizeof(TInt));
#endif
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt BigEndianToCurrent(TInt value)
	{
#if PAL_TRAIT_LITTLE_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

	template<typename TInt> requires TIsIntegralType<TInt>::Value
	FORCE_INLINE static TInt LittleEndianToCurrent(TInt value)
	{
#if PAL_TRAIT_BIG_ENDIAN
		SwapByteOrder(&value, sizeof(TInt));
#endif
		return value;
	}

} // namespace CE

