#pragma once

#include <type_traits>

namespace CE
{

	template <class, template <class> class>
	struct TypeIsInstanceOf : public std::false_type {};

	template <class T, template <class> class U>
	struct TypeIsInstanceOf<U<T>, U> : public std::true_type {};

	template<typename T>
	using RemovePointerFromType = std::remove_pointer_t<T>;

	template<typename T>
	using RemoveConstVolatileFromType = std::remove_cv_t<T>;

	template<typename T>
	class Array;

	template<typename T>
	struct IsArrayType : std::false_type
	{
		typedef void ElementType;
	};

	template<typename T>
	struct IsArrayType<Array<T>> : std::true_type
	{
		typedef T ElementType;
	};


	class ObjectStore;
    
	template<typename T>
	struct IsObjectStoreType : std::false_type
	{
		
	};

	template<>
	struct IsObjectStoreType<ObjectStore> : std::true_type
	{
		
	};

} // namespace CE::Traits

