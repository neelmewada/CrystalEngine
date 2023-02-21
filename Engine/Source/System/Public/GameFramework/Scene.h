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
        
        CE_INLINE u32 GetRootGameObjectCount() const
        {
            return (u32)rootGameObjects.GetSize();
        }
        
        CE_INLINE GameObject* GetRootGameObject(u32 index) const
        {
            return rootGameObjects[index];
        }

        s32 GetRootGameObjectIndex(GameObject* go) const
        {
            if (go == nullptr)
                return -1;
            return rootGameObjects.IndexOf(go);
        }
        
        void AddObject(Object* object);
        void RemoveObject(Object* object);
        void DestroyObject(Object* object);

    protected:
        ObjectStore objects{};
        
        CE::Array<GameObject*> rootGameObjects{};
        
        friend class GameObject;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Scene,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(objects)
        CE_FIELD(rootGameObjects)
    ),
    CE_FUNCTION_LIST()
)
