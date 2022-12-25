#pragma once

#include "Math/Math.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

#include "Object/Object.h"
#include "GameFramework/GameComponent.h"

namespace CE
{

    class SYSTEM_API TransformComponent : public GameComponent
    {
        CE_CLASS(TransformComponent, GameComponent)
    public:
        
        TransformComponent() : GameComponent("Transform")
        {}
        
        // Getters & Setters
        
        CE_INLINE Vec3 GetLocalPosition() const
        {
            return localPosition;
        }
        
        CE_INLINE void SetLocalPositon(Vec3 newLocalPosition)
        {
            this->localPosition = newLocalPosition;
        }
        
        CE_INLINE Quat GetLocalRotation() const
        {
            return localRotation;
        }
        
        CE_INLINE void SetLocalRotation(Quat newLocalRotation)
        {
            this->localRotation = newLocalRotation;
        }
        
        CE_INLINE Vec3 GetLocalScale() const
        {
            return localScale;
        }
        
        CE_INLINE void SetLocalScale(Vec3 newLocalScale)
        {
            this->localScale = newLocalScale;
        }
        
    protected:
        Vec3 localPosition{0, 0, 0};
        Quat localRotation{0, 0, 0, 0};
        Vec3 localScale{1, 1, 1};
        
        friend class GameObject;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, TransformComponent,
    CE_SUPER(CE::GameComponent),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(localPosition)
        CE_FIELD(localRotation)
        CE_FIELD(localScale)
    ),
    CE_FUNCTION_LIST()
)
