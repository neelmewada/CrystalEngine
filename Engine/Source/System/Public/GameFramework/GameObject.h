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

        void AddComponent(GameComponent* component);
        void RemoveComponent(GameComponent* component);
        
        template<typename T> requires std::is_base_of<GameComponent, T>::value
        GameComponent* AddComponent()
        {
            return AddComponent(TYPEID(T));
        }

        GameComponent* AddComponent(TypeId typeId);

        virtual void Tick(f32 deltaTime);

    protected:
        CE::Array<GameComponent*> components{};
        Scene* owner = nullptr;
        
        friend class Scene;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, GameObject,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(components)
    ),
    CE_FUNCTION_LIST()
)
