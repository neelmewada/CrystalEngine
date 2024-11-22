#pragma once

#include "Types/UUID.h"

#include <functional>

namespace std
{
#if PLATFORM_MAC
    template <int _Np>
    using _Ph = struct std::placeholders::__ph<_Np>;
#endif
}

namespace CE
{

    typedef u64 DelegateHandle;
    
    namespace Internal
    {
        template<class>
        constexpr bool AlwaysFalse = false;

        template<class T>
        class DelegateBase
        {
            static_assert(AlwaysFalse<T>, "Delegate should always be passed with function template arguments.");
        };

        template<class TRetType, class... TArgs>
        class DelegateBase<TRetType(TArgs...)>
        {
        public:
            using RetType = TRetType;

            DelegateBase() : impl(nullptr), handle(0)
            {}

            DelegateBase(const std::function<TRetType(TArgs...)>& func) : impl(func), handle(GenerateRandomU64())
            {}

            DelegateBase(TRetType(* func)(TArgs...)) : impl(func), handle(GenerateRandomU64())
            {}

            template<typename F>
            DelegateBase(const F& lamda) : impl(lamda), handle(GenerateRandomU64())
            {}

            template<typename ReturnType, typename ClassOrStruct, typename... Args>
            DelegateBase(ReturnType(ClassOrStruct::* function)(Args...), ClassOrStruct* instance) 
                : DelegateBase(function, instance, std::make_index_sequence<sizeof...(Args)>())
            {

            }

            template<typename ReturnType, typename ClassOrStruct, typename... Args>
            DelegateBase(ReturnType(ClassOrStruct::* function)(Args...) const, ClassOrStruct* instance)
                : DelegateBase(function, instance, std::make_index_sequence<sizeof...(Args)>())
            {

            }

            DelegateBase(const DelegateBase<TRetType(TArgs...)>& copy)
            {
                this->impl = copy.impl;
                this->handle = copy.handle;
            }

            TRetType operator()(TArgs... args) const
            {
                return impl(args...);
            }

            bool IsValid() const
            {
                return impl != nullptr && handle != 0;
            }

            TRetType Invoke(TArgs... args) const
            {
                return impl(args...);
            }

            TRetType InvokeIfValid(TArgs... args) const
            {
                if (!IsValid())
                {
                    if constexpr (std::is_same_v<TRetType, void>)
                    {
                        return;
                    }
                    else
                    {
                        return {};
                    }
                }
                return impl(args...);
            }

        private:

#if PLATFORM_LINUX
            template<int Num>
            using Placeholder = std::_Placeholder<Num>;
#else
            template<int Num>
            using Placeholder = std::_Ph<Num>;
#endif

        protected:

            template<typename ReturnType, typename ClassOrStruct, typename... Args, std::size_t... Is>
            DelegateBase(ReturnType(ClassOrStruct::* function)(Args...), ClassOrStruct* instance, std::index_sequence<Is...>) : handle(GenerateRandomU64())
            {
                impl = std::bind(function, instance, Placeholder<Is + 1>()...);
            }

            template<typename ReturnType, typename ClassOrStruct, typename... Args, std::size_t... Is>
            DelegateBase(ReturnType(ClassOrStruct::* function)(Args...) const, ClassOrStruct* instance, std::index_sequence<Is...>) : handle(GenerateRandomU64())
            {
                impl = std::bind(function, instance, Placeholder<Is + 1>()...);
            }

            std::function<TRetType(TArgs...)> impl = nullptr;

            DelegateHandle handle;
        };

        

    }

} // namespace CE
