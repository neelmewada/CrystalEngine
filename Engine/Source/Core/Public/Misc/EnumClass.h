#pragma once


#define ENUM_CLASS_FLAGS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
	inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
	inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }\
	inline			 bool  operator==(Enum E, s32 Rhs) { return E == (Enum)Rhs; }\
	inline			 bool  operator!=(Enum E, s32 Rhs) { return E != (Enum)Rhs; }\
    inline           bool  operator==(Enum E, s64 Rhs) { return E == (Enum)Rhs; }\
    inline           bool  operator!=(Enum E, s64 Rhs) { return E != (Enum)Rhs; }

#define FRIEND_ENUM_CLASS_FLAGS(Enum) \
	friend           Enum& operator|=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator&=(Enum& Lhs, Enum Rhs); \
	friend           Enum& operator^=(Enum& Lhs, Enum Rhs); \
	friend constexpr Enum  operator| (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator& (Enum  Lhs, Enum Rhs); \
	friend constexpr Enum  operator^ (Enum  Lhs, Enum Rhs); \
	friend constexpr bool  operator! (Enum  E); \
	friend constexpr Enum  operator~ (Enum  E);

template<typename Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
	return ( ( ( __underlying_type(Enum) )Flags ) & ( __underlying_type(Enum) )Contains ) == ( ( __underlying_type(Enum) )Contains );
}

template<typename Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	return ( ( ( __underlying_type(Enum) )Flags ) & ( __underlying_type(Enum) )Contains ) != 0;
}

template<typename Enum>
constexpr bool EnumHasFlag(Enum Flags, Enum Contains)
{
	return ( ( ( __underlying_type(Enum) )Flags ) & ( __underlying_type(Enum) )Contains ) != 0;
}

template<typename Enum>
constexpr void EnumAddFlags(Enum& Flags, Enum FlagsToAdd)
{
	Flags |= FlagsToAdd;
}

template<typename Enum>
constexpr void EnumRemoveFlags(Enum& Flags, Enum FlagsToRemove)
{
	Flags &= ~FlagsToRemove;
}
