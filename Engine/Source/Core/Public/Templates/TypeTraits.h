#pragma once

#include <type_traits>

#include "CrystalTemplate.h"

namespace CE
{
	template<typename T>
	struct TIsIntegralType : TFalseType {};

	template<>
	struct TIsIntegralType<u8> : TTrueType {};
	template<>
	struct TIsIntegralType<u16> : TTrueType {};
	template<>
	struct TIsIntegralType<u32> : TTrueType {};
	template<>
	struct TIsIntegralType<u64> : TTrueType {};
	template<>
	struct TIsIntegralType<s8> : TTrueType {};
	template<>
	struct TIsIntegralType<s16> : TTrueType {};
	template<>
	struct TIsIntegralType<s32> : TTrueType {};
	template<>
	struct TIsIntegralType<s64> : TTrueType {};

	template <class, template <class> class>
	struct TypeIsInstanceOf : TFalseType {};

	template <class T, template <class> class U>
	struct TypeIsInstanceOf<U<T>, U> : TTrueType {};

	template<typename T>
	using RemovePointerFromType = std::remove_pointer_t<T>;

	template<typename T>
	using RemoveConstVolatileFromType = std::remove_cv_t<T>;

	template<typename T>
	using RemoveReferenceFromType = std::remove_reference_t<T>;

	template<class TBase, class TDerived>
	using TIsBaseClassOf = TBoolConst<__is_base_of(TBase, TDerived)>;

	template<typename T1, typename T2>
	struct TIsSameType : TFalseType {};

	template<typename T>
	struct TIsSameType<T, T> : TTrueType {};

    template<typename T>
    struct TIsTemplate : TFalseType {};

    template<typename T, template <typename> class U>
    struct TIsTemplate<U<T>> : TTrueType {};

	template<typename T>
	class Array;

	template<typename T>
	struct TIsArray : TFalseType
	{
		typedef void ElementType;
	};

	template<typename T>
	struct TIsArray<Array<T>> : TTrueType
	{
		typedef T ElementType;
	};

    template<typename T>
    struct TIsEnum : TBoolConst<__is_enum(T)> {};

    template<typename T, bool IsEnum = TIsEnum<T>::Value>
    struct TEnumUnderlyingType : std::underlying_type<T>
    {
	    typedef void Type;
    };

	template<typename T>
	struct TEnumUnderlyingType<T, true> : std::underlying_type<T>
	{
		typedef __underlying_type(T) Type;
	};

	class ObjectStore;
    
	template<typename T>
	struct IsObjectStoreType : TFalseType {};

	template<>
	struct IsObjectStoreType<ObjectStore> : TTrueType {};

	template<typename T>
	struct TIsAbstract : TBoolConst<__is_abstract(T)> {};

} // namespace CE::Traits

