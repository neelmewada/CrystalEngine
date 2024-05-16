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

		bool IsSceneComponent() const final { return true; }

		bool IsEnabled() const final;

		/// @brief Add the passed scene component as a child of receiving scene component.
		/// @param component: Component to add as child.
		void SetupAttachment(SceneComponent* component);

		void DetachComponent(SceneComponent* component);

		bool ComponentExistsRecursive(SceneComponent* component);
		bool ComponentExists(SceneComponent* component);

		Vec3 GetPosition() const { return globalPosition; }

		Vec3 GetLocalPosition() const { return localPosition; }

		Vec3 GetLocalEulerAngles() const { return localEulerAngles; }

		Vec3 GetLocalScale() const { return localScale; }

		void SetLocalPosition(const Vec3& value) { localPosition = value; SetDirty(); }

		void SetLocalEulerAngles(const Vec3& value) { localEulerAngles = value; SetDirty(); }

		void SetLocalScale(const Vec3& value) { localScale = value; SetDirty(); }

		void OnBeginPlay() override;

		void Tick(f32 delta) override;

		const Matrix4x4& GetTransform() const { return transform; }

		const Matrix4x4& GetLocalTransform() const { return localTransform; }

		const Vec3& GetForwardVector() const { return forwardVector; }

		const Vec3& GetUpwardVector() const { return upwardVector; }

		const Vec3& GetRightwardVector() const { return rightwardVector; }

		u32 GetAttachedComponentCount() const { return attachedComponents.GetSize(); }

		SceneComponent* GetAttachedComponent(u32 index) const { return attachedComponents[index]; }

		SceneComponent* GetParentComponent() const { return parentComponent; }

    protected:

		bool IsTransformUpdated() const { return transformUpdated; }

	private:

		void UpdateTransformInternal();

		bool IsDirty();

		void SetDirty();

		FIELD(ReadOnly)
		Array<SceneComponent*> attachedComponents{};

		FIELD(ReadOnly)
		SceneComponent* parentComponent = nullptr;

		FIELD(EditAnywhere, Category = "Transform", Display = "Position", CategoryOrder = "-1")
		Vec3 localPosition{};

		FIELD(EditAnywhere, Category = "Transform", Display = "Rotation")
		Vec3 localEulerAngles{};

		FIELD(EditAnywhere, Category = "Transform", Display = "Scale")
		Vec3 localScale = Vec3(1, 1, 1);

		Quat localRotation = Quat::EulerDegrees(0, 0, 0);

		FIELD(ReadOnly)
		Vec3 globalPosition{};

		FIELD(ReadOnly)
		Vec3 forwardVector{};

		FIELD(ReadOnly)
		Vec3 upwardVector{};

		FIELD(ReadOnly)
		Vec3 rightwardVector{};

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
