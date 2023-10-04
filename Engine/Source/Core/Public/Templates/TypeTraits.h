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

	template<typename T>
	struct TIsNumericType : TFalseType {};

	template<>
	struct TIsNumericType<u8> : TTrueType {};
	template<>
	struct TIsNumericType<u16> : TTrueType {};
	template<>
	struct TIsNumericType<u32> : TTrueType {};
	template<>
	struct TIsNumericType<u64> : TTrueType {};
	template<>
	struct TIsNumericType<s8> : TTrueType {};
	template<>
	struct TIsNumericType<s16> : TTrueType {};
	template<>
	struct TIsNumericType<s32> : TTrueType {};
	template<>
	struct TIsNumericType<s64> : TTrueType {};
	template<>
	struct TIsNumericType<f32> : TTrueType {};
	template<>
	struct TIsNumericType<f64> : TTrueType {};

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
	class SubClassType;

	template<typename T>
	struct TIsArray : TFalseType
	{
        using ElementType = void;
	};

	template<typename T>
	struct TIsArray<Array<T>> : TTrueType
	{
		using ElementType = T;
	};

	template<typename T>
	struct TIsSubClassType : TFalseType
	{
		using RequiredType = void;
	};

	template<typename T>
	struct TIsSubClassType<SubClassType<T>> : TTrueType
	{
		using RequiredType = T;
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

	template<typename T, bool IsEnum = TIsEnum<T>::Value, bool IsArray = TIsArray<T>::Value>
	struct TGetUnderlyingType : TFalseType
	{
		typedef void Type;
	};

	template<typename T>
	struct TGetUnderlyingType<T, true, false> : TTrueType // ENUM
	{
		typedef __underlying_type(T) Type;
	};

	template<typename T>
	struct TGetUnderlyingType<Array<T>, false, true> : TTrueType // Array<T>
	{
		typedef T Type;
	};

	template<typename T>
	struct TGetUnderlyingType<SubClassType<T>, false, false> : TTrueType // SubClassType<T>
	{
		typedef T Type;
	};

	class ObjectMap;
    
	template<typename T>
	struct IsObjectStoreType : TFalseType {};

	template<>
	struct IsObjectStoreType<ObjectMap> : TTrueType {};

	template<typename T>
	struct TIsAbstract : TBoolConst<__is_abstract(T)> {};

	template<typename T>
	struct TIsClass : TBoolConst<__is_class(T) && T::IsClass()> {};

	template<typename T>
	struct TIsStruct : TBoolConst<__is_class(T) && T::IsStruct()> {};

	template<typename What, typename... Args>
	struct TIsTypePresent : TBoolConst<(std::is_same_v<What, Args> or ...)>
	{
		
	};

	// Function

	template <typename T>
	struct FunctionTraits : public FunctionTraits<decltype(&T::operator())>
	{};

	// For generic types, directly use the result of the signature of its 'operator()'

	template <typename TClassType, typename TReturnType, typename... Args>
	struct FunctionTraits<TReturnType(TClassType::*)(Args...) const> // we specialize for pointers to const member function
	{
		enum { NumArgs = sizeof...(Args) };

		typedef TReturnType ReturnType;

		typedef std::tuple<Args...> Tuple;

		template <SIZE_T i>
		struct Arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type Type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template <typename TClassType, typename TReturnType, typename... Args>
	struct FunctionTraits<TReturnType(TClassType::*)(Args...)> // we specialize for pointers to member function
	{
		enum { NumArgs = sizeof...(Args) };

		typedef TReturnType ReturnType;

		typedef std::tuple<Args...> Tuple;

		template <SIZE_T i>
		struct Arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type Type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};

	template <typename TReturnType, typename... Args>
	struct FunctionTraits<TReturnType(*)(Args...)> // we specialize for pointers to global functions
	{
		enum { NumArgs = sizeof...(Args) };

		typedef TReturnType ReturnType;

		typedef std::tuple<Args...> Tuple;

		template <SIZE_T i>
		struct Arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type Type;
			// the i-th argument is equivalent to the i-th tuple element of a tuple
			// composed of those arguments.
		};
	};


	template<typename T, typename = void>
	struct TStructReleaseFunction : TFalseType
	{
		static void Release(T* instance) {} // Do nothing
	};

	template<typename T>
	struct TStructReleaseFunction<T, std::void_t<decltype(std::declval<T>().Release())>> : TTrueType
	{
		static void Release(T* instance)
		{
			instance->T::Release(); // Release function exists
		}
	};

	class ObjectInitializer;

	/// True if type has default constructor and is NOT abstract
	template<typename T, typename = void>
	struct THasDefaultCtor : TFalseType
	{

	};

	template<typename T>
	struct THasDefaultCtor<T, std::void_t<decltype(T())>> : TTrueType
	{

	};

	/// True if type has default constructor and is NOT abstract
	template<typename T, typename = void>
	struct THasObjectInitCtor : TFalseType
	{

	};

	template<typename T>
	struct THasObjectInitCtor<T, std::void_t<decltype(T(CE::ObjectInitializer()))>> : TTrueType
	{

	};

	template<typename T>
	struct TIsCopyConstructible : TBoolConst<std::is_copy_constructible<T>::value>
	{

	};

	template<typename T, typename = void>
	struct THasGetHashFunction : TFalseType
	{
		static SIZE_T GetHash(const T* instance) { return 0; }
	};

	template<typename T>
	struct THasGetHashFunction<T, std::void_t<decltype(std::declval<T>().GetHash())>> : TTrueType
	{
		static SIZE_T GetHash(const T* instance) { return instance->GetHash(); }
	};

	template<typename T, typename = void>
	struct THasOnBeforeSerializeFunction : TFalseType
	{
		static void OnBeforeSerialize(T* instance) {} // Do nothing
	};

	template<typename T>
	struct THasOnBeforeSerializeFunction<T, std::void_t<decltype(std::declval<T>().GetHash())>> : TTrueType
	{
		static void OnBeforeSerialize(T* instance) { return instance->OnBeforeSerialize(); }
	};

	template<typename T, typename = void>
	struct THasOnAfterDeserializeFunction : TFalseType
	{
		static void OnAfterDeserialize(T* instance) {} // Do nothing
	};

	template<typename T>
	struct THasOnAfterDeserializeFunction<T, std::void_t<decltype(std::declval<T>().GetHash())>> : TTrueType
	{
		static void OnAfterDeserialize(T* instance) { return instance->OnAfterDeserialize(); }
	};

	template<typename T>
	struct TTypeDestructor
	{
		static void Invoke(void* instance) 
		{
			static_assert(std::is_destructible<T>::value);
			((T*)instance)->~T();
		}
	};

} // namespace CE::Traits

