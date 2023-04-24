#pragma once

#include "Object/Object.h"
#include "Object/SystemObject.h"
#include "Object/ObjectStore.h"

#include "Containers/HashSet.h"

namespace CE
{
    class GameObject;

    CLASS()
    class SYSTEM_API Scene : public CE::Object
    {
        CE_CLASS(Scene, CE::Object)
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

        CE_INLINE s32 GetRootGameObjectIndex(GameObject* go) const
        {
            if (go == nullptr)
                return -1;
            return rootGameObjects.IndexOf(go);
        }

        CE_INLINE CE::Array<Object*> GetObjectsOfType(TypeId typeId)
        {
            if (!objectTypeToArrayMap.KeyExists(typeId))
            {
                return {};
            }
            return objectTypeToArrayMap[typeId];
        }
        
        void AddObject(Object* object);
        void RemoveObject(Object* object);
        void DestroyObject(Object* object);

        void DestroyAll();
        
    protected:
        FIELD(Hidden)
        ObjectStore objects{};
        
        FIELD(Hidden)
        CE::Array<GameObject*> rootGameObjects{};

        CE::HashMap<TypeId, CE::Array<Object*>> objectTypeToArrayMap{};
        
        friend class GameObject;
    };
    
} // namespace CE

#include "Scene.rtti.h"

