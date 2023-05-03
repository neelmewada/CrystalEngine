#pragma once

#include <type_traits>

namespace CE
{
	template<bool TValue>
	struct TTBoolConst
	{
		static constexpr bool value = TValue;
	};
	
	struct TTFalseType : TTBoolConst<false> {};

	struct TTTrueType : TTBoolConst<true> {};

	template <class, template <class> class>
	struct TypeIsInstanceOf : std::false_type {};

	template <class T, template <class> class U>
	struct TypeIsInstanceOf<U<T>, U> : std::true_type {};

	template<typename T>
	using RemovePointerFromType = std::remove_pointer_t<T>;

	template<typename T>
	using RemoveConstVolatileFromType = std::remove_cv_t<T>;

	template<typename T>
	using RemoveReferenceFromType = std::remove_reference_t<T>;

	template<class TBase, class TDerived>
	using IsBaseClassOf = std::is_base_of<TBase, TDerived>;

	template<typename T1, typename T2>
	struct IsSameType : TTFalseType {};

	template<typename T>
	struct IsSameType<T, T> : TTTrueType {};

    template<typename T>
    struct IsTemplate : TTFalseType {};

    template<typename T, template <typename> class U>
    struct IsTemplate<U<T>> : TTTrueType {};

	template<typename T>
	class Array;

	template<typename T>
	struct IsArrayType : TTFalseType
	{
		typedef void ElementType;
	};

	template<typename T>
	struct IsArrayType<Array<T>> : TTTrueType
	{
		typedef T ElementType;
	};

    template<typename T>
    struct IsEnumType : std::is_enum<T> {};

    template<typename T>
    struct EnumUnderlyingType : std::underlying_type<T> {};

	class ObjectStore;
    
	template<typename T>
	struct IsObjectStoreType : std::false_type {};

	template<>
	struct IsObjectStoreType<ObjectStore> : std::true_type {};

} // namespace CE::Traits

