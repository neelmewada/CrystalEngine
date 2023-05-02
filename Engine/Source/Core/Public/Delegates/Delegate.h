#pragma once

#include "DelegateBase.h"

namespace CE
{
    
    template<typename T>
    class Delegate : public Internal::DelegateBase<T>
    {
    public:
        Delegate() : Internal::DelegateBase<T>(nullptr)
        {}

        Delegate(const std::function<T>& func) : Internal::DelegateBase<T>(func)
        {}

        template<typename F>
        Delegate(const F& lambda) : Internal::DelegateBase<T>(lambda)
        {}

        Delegate(const Delegate<T>& copy) : Internal::DelegateBase<T>((const Internal::DelegateBase<T>&)copy)
        {}

        template<class ReturnType, class ClassOrStruct, typename... Args>
        Delegate(ReturnType(ClassOrStruct::* function)(Args...), ClassOrStruct* instance) : Internal::DelegateBase<T>(function, instance)
        {

        }

        TypeId GetTargetType() const 
        {
            if (Internal::DelegateBase<T>::impl == nullptr)
                return 0;
            return Internal::DelegateBase<T>::impl.target_type().hash_code(); 
        }

        auto GetTarget() const
        {
            return Internal::DelegateBase<T>::impl.template target<T>();
        }

        inline bool operator==(const Delegate<T>& other)
        {
            return GetHandle() == other.GetHandle();
        }

        inline bool operator!=(const Delegate<T>& other)
        {
            return GetHandle() != other.GetHandle();
        }

        bool IsValid() const
        {
            return Internal::DelegateBase<T>::IsValid();
        }

        DelegateHandle GetHandle() const
        {
            return Internal::DelegateBase<T>::handle;
        }
    };

    template<typename ReturnType, typename ClassOrStruct, typename... Args>
    inline Delegate<ReturnType(Args...)> MemberDelegate(ReturnType(ClassOrStruct::* function)(Args...), ClassOrStruct* instance)
    {
        return Delegate<ReturnType(Args...)>(function, instance);
    }

} // namespace CE
