#pragma once

namespace CE
{
    template<typename T> requires TIsBaseClassOf<Object, T>::Value
	class Ref final
	{
	public:
        
        Ref()
        {
            
        }
        
        Ref(T* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddStrongRef();
            }
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and TIsBaseClassOf<Object, U>::Value and
            (not std::is_same_v<T, U>)
        Ref(U* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddStrongRef();
            }
        }
        
        Ref(const Ref& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddStrongRef();
            }
        }
        
        Ref& operator=(const Ref& copy)
        {
            if (control)
            {
                control->ReleaseStrongRef();
            }
            control = copy.control;
            if (control)
            {
                control->AddStrongRef();
            }
            return *this;
        }
        
        Ref& operator=(T* newObject)
        {
            if (control)
            {
                control->ReleaseStrongRef();
            }
            control = nullptr;
            if (newObject != nullptr)
            {
                control = newObject->control;
            }
            if (control != nullptr)
            {
                control->AddStrongRef();
            }
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and TIsBaseClassOf<Object,U>::Value and
            (not std::is_same_v<T, U>)
        Ref& operator=(U* newObject)
        {
            if (control)
            {
                control->ReleaseStrongRef();
            }
            control = nullptr;
            if (newObject != nullptr)
            {
                control = newObject->control;
            }
            if (control != nullptr)
            {
                control->AddStrongRef();
            }
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value
        Ref(const Ref<U>& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddStrongRef();
            }
        }
        
        Ref(Ref&& move)
        {
            control = move.control;
            move.control = nullptr;
        }
        
        ~Ref()
        {
            if (control)
            {
                control->ReleaseStrongRef();
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
        
        inline bool operator==(const Ref& rhs) const
        {
            return control == rhs.control;
        }
        
        inline bool operator!=(const Ref& rhs) const
        {
            return control != rhs.control;
        }

	private:

		Internal::RefCountControl* control = nullptr;
        
        template<typename _T> requires TIsBaseClassOf<Object, _T>::Value
        friend class WeakRef;
	};

	
} // namespace CE
