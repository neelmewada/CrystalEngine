#pragma once

#include <type_traits>

namespace CE::Traits
{

	template <class, template <class> class>
	struct TypeIsInstanceOf : public std::false_type {};

	template <class T, template <class> class U>
	struct TypeIsInstanceOf<U<T>, U> : public std::true_type {};

	template<typename T>
	using RemovePointerFromType = std::remove_pointer_t<T>;

	template<typename T>
	using RemoveConstVolatileFromType = std::remove_cv_t<T>;

    
} // namespace CE::Traits

