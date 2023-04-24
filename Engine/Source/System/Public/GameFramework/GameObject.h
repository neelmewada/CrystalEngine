#pragma once

#include "Object/Object.h"
#include "Component/Component.h"

namespace CE
{
    class GameComponent;
    class Scene;

    CLASS()
    class SYSTEM_API GameObject : public Object
    {
        CE_CLASS(GameObject, CE::Object)
    public:
        GameObject();
        GameObject(CE::Name name);
        GameObject(Scene* scene, CE::Name name = TEXT(GameObject));
        
        virtual ~GameObject();

        GameComponent* AddComponent(GameComponent* component);
        void RemoveComponent(GameComponent* component);
        
        template<typename TComponent> requires std::is_base_of<GameComponent, TComponent>::value
        TComponent* AddComponent()
        {
            return (TComponent*)AddComponent(TComponent::Type());
        }

        GameComponent* AddComponent(ClassType* componentClass);
        GameComponent* AddComponent(TypeId componentTypeId);

        void RemoveComponent(ClassType* componentClass);
        void RemoveComponent(TypeId componentTypeId);

        template<typename TComponent> requires std::is_base_of<GameComponent, TComponent>::value
        void RemoveComponent()
        {
            RemoveComponent(TComponent::Type());
        }


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

        Scene* GetOwner() const { return owner; }
        
    protected:

        void OnSubComponentAdded(GameComponent* subComponent);
        void OnSubComponentRemoved(GameComponent* subComponent);

        FIELD()
        Scene* owner = nullptr;
        
        FIELD()
        Array<GameComponent*> components{};

        FIELD()
        GameObject* parent = nullptr;
        
        FIELD()
        Array<GameObject*> children{};
        
        friend class Scene;
        friend class GameComponent;
    };
    
} // namespace CE

#include "GameObject.rtti.h"
