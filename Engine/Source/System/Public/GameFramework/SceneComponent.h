#pragma once

namespace CE
{
    CLASS()
	class SYSTEM_API SceneComponent : public ActorComponent
	{
		CE_CLASS(SystemComponent, ActorComponent)
	public:
		SceneComponent();

		// - Public API -

		bool IsSceneComponent() const override final { return true; }

		/// @brief Add the passed scene component as a child of receiving scene component.
		/// @param component: Component to add as child.
		void SetupAttachment(SceneComponent* component);

		bool ComponentExists(SceneComponent* component);

		inline Vec3 GetLocalPosition() const { return localPosition; }

		inline Vec3 GetLocalEulerAngles() const { return localEulerAngles; }

		inline Vec3 GetLocalScale() const { return localScale; }

		inline void SetLocalPosition(const Vec3& value) { localPosition = value; }

		inline void SetLocalEulerAngles(const Vec3& value) { localEulerAngles = value; }

		inline void SetLocalScale(const Vec3& value) { localScale = value; }

		void Tick(f32 delta) override;

	private:

		FIELD()
		Array<SceneComponent*> attachedComponents{};

		FIELD()
		SceneComponent* parentComponent = nullptr;

		FIELD(Category = Transform, Display = "Position")
		Vec3 localPosition{};

		FIELD(Category = Transform, Display = "Rotation")
		Vec3 localEulerAngles{};

		FIELD(Category = Transform, Display = "Scale")
		Vec3 localScale{};
        
        Matrix4x4 transform{};
	};

} // namespace CE

#include "SceneComponent.rtti.h"
