#pragma once

#include "Math/Math.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"

#include "Object/Object.h"
#include "GameFramework/GameComponent.h"

namespace CE
{
    CLASS(AllowMultiple = false, Display = Transform)
    class SYSTEM_API TransformComponent : public GameComponent
    {
        CE_CLASS(TransformComponent, CE::GameComponent)
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
            OnComponentValuesUpdated();
        }
        
        CE_INLINE Quat GetLocalRotation() const
        {
            return Quat::FromEuler(localRotation);
        }
        
        CE_INLINE void SetLocalRotation(Quat newLocalRotation)
        {
            this->localRotation = newLocalRotation.ToEuler();
            OnComponentValuesUpdated();
        }

        CE_INLINE Vec3 GetLocalEuler() const
        {
            return localRotation;
        }

        CE_INLINE void SetLocalEuler(Vec3 euler)
        {
            this->localRotation = euler;
            OnComponentValuesUpdated();
        }
        
        CE_INLINE Vec3 GetLocalScale() const
        {
            return localScale;
        }
        
        CE_INLINE void SetLocalScale(Vec3 newLocalScale)
        {
            this->localScale = newLocalScale;
            OnComponentValuesUpdated();
        }
        
    protected:
        FIELD(Display = Position)
        Vec3 localPosition{0, 0, 0};

        FIELD(Display = Rotation)
        Vec3 localRotation{0, 0, 0};

        FIELD(Display = Scale)
        Vec3 localScale{1, 1, 1};
        
        friend class GameObject;
    };
    
} // namespace CE


#include "TransformComponent.rtti.h"
