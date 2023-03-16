#pragma once

#include "Object/Object.h"
#include "Component/Component.h"

namespace CE
{
    class GameComponent;
    class Scene;

    class SYSTEM_API GameObject : public Object
    {
        CE_CLASS(GameObject, Object);
    public:
        GameObject();
        GameObject(CE::Name name);
        GameObject(Scene* scene);
        
        virtual ~GameObject();

        GameComponent* AddComponent(GameComponent* component);
        void RemoveComponent(GameComponent* component);
        
        template<typename T> requires std::is_base_of<GameComponent, T>::value
        T* AddComponent()
        {
            return (T*)AddComponent(TYPEID(T));
        }

        GameComponent* AddComponent(ClassType* componentType);
        GameComponent* AddComponent(TypeId typeId);

        virtual void Tick(f32 deltaTime);

        CE_INLINE u32 GetChildrenCount() const
        {
            return (u32)children.GetSize();
        }
        
        CE_INLINE GameObject* GetChildAt(u32 index) const
        {
            return children[index];
        }

        CE_INLINE u32 GetComponentCount() const
        {
            return components.GetSize();
        }

        CE_INLINE GameComponent* GetComponentAt(u32 index) const
        {
            return components[index];
        }

        GameComponent* GetComponent(TypeId componentTypeId) const;

        template<typename ComponentType>
        CE_INLINE GameComponent* GetComponent()
        {
            TypeId componentTypeId = ComponentType::Type()->GetTypeId();
            return GetComponent(componentTypeId);
        }

        bool HasComponent(TypeId componentTypeId);

        template<typename ComponentType>
        CE_INLINE bool HasComponent()
        {
            return HasComponent(ComponentType::Type()->GetTypeId());
        }
        
        void AddChild(GameObject* child);
        void RemoveChild(GameObject* child);
        
        CE_INLINE GameObject* GetParent() const
        {
            return parent;
        }
        
        s32 GetIndexInParent();
        s32 GetChildIndex(GameObject* child);
        
    protected:
        Scene* owner = nullptr;
        
        CE::Array<GameComponent*> components{};
        GameObject* parent = nullptr;
        
        CE::Array<GameObject*> children{};
        
        friend class Scene;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, GameObject,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(owner)
        CE_FIELD(components)
        CE_FIELD(parent)
        CE_FIELD(children)
    ),
    CE_FUNCTION_LIST()
)
