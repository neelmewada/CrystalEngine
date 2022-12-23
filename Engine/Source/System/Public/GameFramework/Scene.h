#pragma once

#include "Object/Object.h"
#include "Object/SystemObject.h"
#include "Object/ObjectStore.h"

#include "Containers/HashSet.h"

namespace CE
{
    class GameObject;

    class SYSTEM_API Scene : public Object
    {
        CE_CLASS(Scene)

    public:
        Scene();
        Scene(Name name);
        virtual ~Scene();
        
        virtual void Tick(f32 deltaTime);
        
        void AddGameObject(GameObject* gameObject);
        void DestroyGameObject(GameObject* gameObject);

    protected:
        ObjectStore objects{};
        
        CE::Array<GameObject*> gameObjects{};
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Scene,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(objects)
        CE_FIELD(gameObjects)
    ),
    CE_FUNCTION_LIST()
)
