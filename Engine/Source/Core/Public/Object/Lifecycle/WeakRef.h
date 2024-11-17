#pragma once

namespace CE
{
    
    template<typename T = Object> requires TIsBaseClassOf<Object, T>::Value
    class WeakRef final
    {
    public:
        WeakRef()
        {
            
        }
        
        WeakRef(T* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = object;
#endif
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
        WeakRef(U* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = (T*)object;
#endif
        }
        
        WeakRef(const WeakRef& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = copy.ptr;
#endif
        }
        
        WeakRef& operator=(const WeakRef& copy)
        {
            if (control)
            {
                control->ReleaseWeakRef();
            }
            control = copy.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = copy.ptr;
#endif
            return *this;
        }
        
        WeakRef& operator=(T* newObject)
        {
            if (control)
            {
                control->ReleaseWeakRef();
            }
            control = nullptr;
            if (newObject != nullptr)
            {
                control = newObject->control;
            }
            if (control != nullptr)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = newObject;
#endif
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
        WeakRef& operator=(U* newObject)
        {
            if (control)
            {
                control->ReleaseWeakRef();
            }
            control = nullptr;
            if (newObject != nullptr)
            {
                control = newObject->control;
            }
            if (control != nullptr)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = (T*)newObject;
#endif
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value
        WeakRef(const WeakRef<U>& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = copy.ptr;
#endif
        }
        
        WeakRef(const Ref<T>& from)
        {
            control = from.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = from.ptr;
#endif
        }
        
        WeakRef& operator=(const Ref<T>& from)
        {
            if (control)
            {
                control->ReleaseWeakRef();
            }
            control = from.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = from.ptr;
#endif
            return *this;
        }

        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
        WeakRef(const Ref<U>& from)
        {
            control = from.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = from.ptr;
#endif
        }

        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
        WeakRef& operator=(const Ref<U>& from)
        {
            if (control)
            {
                control->ReleaseWeakRef();
            }
            control = from.control;
            if (control)
            {
                control->AddWeakRef();
            }
#if CE_BUILD_DEBUG
            ptr = from.ptr;
#endif
            return *this;
        }
        
        WeakRef(WeakRef&& move)
        {
            control = move.control;
            move.control = nullptr;
#if CE_BUILD_DEBUG
            ptr = move.ptr;
            move.ptr = nullptr;
#endif
        }
        
        ~WeakRef()
        {
            if (control)
            {
                control->ReleaseWeakRef();
                control = nullptr;
            }
#if CE_BUILD_DEBUG
            ptr = nullptr;
#endif
        }
        
        T* Get() const
        {
            Object* object = nullptr;
            if (control == nullptr)
            {
                return nullptr;
            }
            object = control->GetObject();
            if (object == nullptr)
            {
                return nullptr;
            }
            return (T*)object;
        }
        
        Ref<T> Lock() const
        {
            return Ref<T>(Get());
        }
        
    	operator T*() const
        {
            return Get();
        }
        
        T& operator*() const
        {
            Object* object = nullptr;
            if (control == nullptr)
            {
                throw NullPointerException("Reference is NULL!");
            }
            object = control->GetObject();
            if (object == nullptr)
            {
                throw NullPointerException("Object is destroyed!");
            }
            return *(T*)object;
        }
        
        inline T* operator->() const
        {
            Object* object = nullptr;
            if (control == nullptr)
            {
                throw NullPointerException("Reference is NULL!");
            }
            object = control->GetObject();
            if (object == nullptr)
            {
                throw NullPointerException("Object is destroyed!");
            }
            return (T*)object;
        }
        
        inline bool operator!() const
        {
            return Get() == nullptr;
        }
        
        inline operator bool() const
        {
            return Get() != nullptr;
        }

        inline bool operator==(T* object) const
        {
            return (control == nullptr && object == nullptr) || (control->GetObject() == object);
        }

        inline bool operator!=(T* object) const
        {
            return !operator==(object);
        }
        
        inline bool operator==(const WeakRef& rhs) const
        {
            return control == rhs.control;
        }
        
        inline bool operator!=(const WeakRef& rhs) const
        {
            return control != rhs.control;
        }

        SIZE_T GetHash() const
        {
            return (SIZE_T)control;
        }
        
    private:

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class WeakRef;

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class Ref;

#if CE_BUILD_DEBUG
        T* ptr = nullptr;
#endif
        Internal::RefCountControl* control = nullptr;
    };

    template <typename T> requires TIsBaseClassOf<Object, T>::Value
    Ref<T>::Ref(const WeakRef<T>& weakRef)
    {
        control = weakRef.control;
        if (control)
        {
            control->AddStrongRef();
        }
#if CE_BUILD_DEBUG
        ptr = (T*)weakRef.ptr;
#endif
    }

    template <typename T> requires TIsBaseClassOf<Object, T>::Value
    Ref<T>& Ref<T>::operator=(const WeakRef<T>& weakRef)
    {
        if (control)
        {
            control->ReleaseStrongRef();
        }
        control = weakRef.control;
        if (control)
        {
            control->AddStrongRef();
        }
#if CE_BUILD_DEBUG
        ptr = (T*)weakRef.ptr;
#endif
        return *this;
    }

} // namespace CE
