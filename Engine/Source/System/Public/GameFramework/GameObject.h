#pragma once

#include "Object/Object.h"
#include "Component/Component.h"

namespace CE
{
    class GameComponent;

    class SYSTEM_API GameObject : public Object
    {
        CE_CLASS(GameObject, Object);
    public:
        GameObject();
        GameObject(CE::Name name);
        
        virtual ~GameObject();

        void AddComponent(GameComponent* component);
        void RemoveComponent(GameComponent* component);
        
        template<typename T> requires std::is_base_of<GameComponent, T>::value
        void AddComponent()
        {
            AddComponent(new T);
        }

        void AddComponent(TypeId typeId);
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, GameObject,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    ),
    CE_FUNCTION_LIST()
)
