#pragma once

namespace CE
{
    CLASS()
	class SYSTEM_API SceneComponent : public ActorComponent
	{
		CE_CLASS(SceneComponent, ActorComponent)
	public:
		SceneComponent();

		// - Public API -

		bool IsSceneComponent() const override final { return true; }

		/// @brief Add the passed scene component as a child of receiving scene component.
		/// @param component: Component to add as child.
		void SetupAttachment(SceneComponent* component);

		bool ComponentExists(SceneComponent* component);

		Vec3 GetLocalPosition() const { return localPosition; }

		Vec3 GetLocalEulerAngles() const { return localEulerAngles; }

		Vec3 GetLocalScale() const { return localScale; }

		void SetLocalPosition(const Vec3& value) { localPosition = value; SetDirty(); }

		void SetLocalEulerAngles(const Vec3& value) { localEulerAngles = value; SetDirty(); }

		void SetLocalScale(const Vec3& value) { localScale = value; SetDirty(); }

		void OnBeginPlay() override;

		void Tick(f32 delta) override;

		bool IsDirty();

		const Matrix4x4& GetTransform() const { return transform; }

		const Matrix4x4& GetLocalTransform() const { return localTransform; }

    protected:

		bool IsTransformUpdated() const { return transformUpdated; }

	private:

		void SetDirty();

		FIELD()
		Array<SceneComponent*> attachedComponents{};

		FIELD()
		SceneComponent* parentComponent = nullptr;

		FIELD(Category = Transform, Display = "Position")
		Vec3 localPosition{};

		FIELD(Category = Transform, Display = "Rotation")
		Vec3 localEulerAngles{};

		FIELD(Category = Transform, Display = "Scale")
		Vec3 localScale = Vec3(1, 1, 1);

		Quat localRotation = Quat::EulerDegrees(0, 0, 0);

		b8 isDirty = true;

		b8 transformUpdated = false;
        
        Matrix4x4 transform{};
		Matrix4x4 localTransform{};

		Matrix4x4 localTranslationMat = Matrix4x4::Identity();
		Matrix4x4 localRotationMat = Matrix4x4::Identity();
		Matrix4x4 localScaleMat = Matrix4x4::Identity();
        
        friend class CE::Scene;
	};

} // namespace CE

#include "SceneComponent.rtti.h"
