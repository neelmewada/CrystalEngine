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
        GameObject(Scene* scene, CE::Name name = "GameObject");
        
        virtual ~GameObject();

        virtual void Tick(f32 deltaTime);

        CE_INLINE u32 GetChildrenCount() const
        {
            return (u32)children.GetSize();
        }
        
        CE_INLINE GameObject* GetChildAt(u32 index) const
        {
            return children[index];
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
