#pragma once

namespace CE
{

    template<typename T = Object> requires TIsBaseClassOf<Object, T>::Value
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

#if CE_BUILD_DEBUG
                ptr = object;
#endif
            }
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
        Ref(U* object)
        {
            if (object != nullptr)
            {
                control = object->control;
                control->AddStrongRef();

#if CE_BUILD_DEBUG
                ptr = (T*)object;
#endif
            }
        }
        
        Ref(const Ref& copy)
        {
            control = copy.control;
            if (control)
            {
                control->AddStrongRef();
            }
#if CE_BUILD_DEBUG
            ptr = (T*)copy.ptr;
#endif
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
#if CE_BUILD_DEBUG
            ptr = (T*)copy.ptr;
#endif
            return *this;
        }

        Ref(const WeakRef<T>& weakRef);

        Ref& operator=(const WeakRef<T>& weakRef);
        
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
#if CE_BUILD_DEBUG
            ptr = (T*)newObject;
#endif
            return *this;
        }
        
        template<class U> requires TIsBaseClassOf<T, U>::Value and (not std::is_same_v<T, U>)
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
#if CE_BUILD_DEBUG
            ptr = (T*)newObject;
#endif
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
#if CE_BUILD_DEBUG
            ptr = (T*)copy.ptr;
#endif
        }
        
        Ref(Ref&& move)
        {
            control = move.control;
            move.control = nullptr;
#if CE_BUILD_DEBUG
            ptr = (T*)move.ptr;
            move.ptr = nullptr;
#endif
        }
        
        ~Ref()
        {
            if (control)
            {
                control->ReleaseStrongRef();
                control = nullptr;
            }
#if CE_BUILD_DEBUG
            ptr = nullptr;
#endif
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

        inline bool operator==(T* object) const
        {
            return (control == nullptr && object == nullptr) || (control->GetObject() == object);
        }

        inline bool operator!=(T* object) const
        {
            return !operator==(object);
        }
        
        inline bool operator==(const Ref& rhs) const
        {
            return control == rhs.control;
        }
        
        inline bool operator!=(const Ref& rhs) const
        {
            return control != rhs.control;
        }

        SIZE_T GetHash() const
        {
            return (SIZE_T)control;
        }

	private:

#if CE_BUILD_DEBUG
        T* ptr = nullptr;
#endif
		Internal::RefCountControl* control = nullptr;

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class WeakRef;

        template<typename U> requires TIsBaseClassOf<Object, U>::Value
        friend class Ref;
	};

    
} // namespace CE

