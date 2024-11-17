#pragma once

namespace CE
{
    
    template<typename T> requires TIsBaseClassOf<Object, T>::Value
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
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and TIsBaseClassOf<Object, U>::Value and
            (not std::is_same_v<T, U>)
        WeakRef(U* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddWeakRef();
            }
        }
        
        WeakRef(const WeakRef& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddWeakRef();
            }
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
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and TIsBaseClassOf<Object,U>::Value and
            (not std::is_same_v<T, U>)
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
        }
        
        WeakRef(const Ref<T>& from)
        {
            control = from.control;
            if (control)
            {
                control->AddWeakRef();
            }
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
            return *this;
        }
        
        WeakRef(WeakRef&& move)
        {
            control = move.control;
            move.control = nullptr;
        }
        
        ~WeakRef()
        {
            if (control)
            {
                control->ReleaseWeakRef();
                control = nullptr;
            }
        }
        
        inline T* Get() const
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
        
        inline operator T*() const
        {
            return Get();
        }
        
        inline T& operator*() const
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
        
        inline bool operator==(const WeakRef& rhs) const
        {
            return control == rhs.control;
        }
        
        inline bool operator!=(const WeakRef& rhs) const
        {
            return control != rhs.control;
        }
        
    private:

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class WeakRef;

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class Ref;
        
        Internal::RefCountControl* control = nullptr;
    };

} // namespace CE
