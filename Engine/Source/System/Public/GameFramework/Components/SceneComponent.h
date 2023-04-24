#pragma once

#include "GameFramework/GameComponent.h"

namespace CE
{
	CLASS()
	class SYSTEM_API SceneComponent : public GameComponent
	{
		CE_CLASS(SceneComponent, CE::GameComponent)
	public:
		SceneComponent();
		SceneComponent(Name name);

		CE_INLINE Vec3 GetLocalPosition() const { return localPosition; }
		CE_INLINE void SetLocalPosition(const Vec3& newLocalPosition)
		{
			localPosition = newLocalPosition;
			fire OnComponentValuesUpdated();
		}

		CE_INLINE Vec3 GetLocalEulerAngles() const { return localEuler; }
		CE_INLINE void SetLocalEulerAngles(const Vec3& newLocalEulerAngles)
		{
			localEuler = newLocalEulerAngles;
			fire OnComponentValuesUpdated();
		}

		CE_INLINE Vec3 GetLocalScale() const { return localScale; }
		CE_INLINE void SetLocalScale(const Vec3& newLocalScale)
		{
			localScale = newLocalScale;
			fire OnComponentValuesUpdated();
		}

	private:
		FIELD(Display = Position, Category = Transform)
		Vec3 localPosition{};

		FIELD(Display = Rotation, Category = Transform)
		Vec3 localEuler{};

		FIELD(Display = Scale, Category = Transform)
		Vec3 localScale{};
	};

} // namespace CE

#include "SceneComponent.rtti.h"
