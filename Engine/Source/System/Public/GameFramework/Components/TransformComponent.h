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
            return Quat::FromEuler(localRotation);
        }
        
        CE_INLINE void SetLocalRotation(Quat newLocalRotation)
        {
            this->localRotation = newLocalRotation.ToEuler();
        }

        CE_INLINE Vec3 GetLocalEuler() const
        {
            return localRotation;
        }

        CE_INLINE void SetLocalEuler(Vec3 euler)
        {
            this->localRotation = euler;
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
        Vec3 localRotation{0, 0, 0};
        Vec3 localScale{1, 1, 1};
        
        friend class GameObject;
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, TransformComponent,
    CE_SUPER(CE::GameComponent),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(AllowMultiple = false, Display = Transform),
    CE_FIELD_LIST(
        CE_FIELD(localPosition, Display = Position)
        CE_FIELD(localRotation, Display = Rotation)
        CE_FIELD(localScale, Display = Scale)
    ),
    CE_FUNCTION_LIST()
)
