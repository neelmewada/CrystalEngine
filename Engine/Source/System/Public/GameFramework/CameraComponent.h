#pragma once

namespace CE
{
	ENUM()
	enum class CameraProjection
	{
		Perspective = 0,
		Orthogonal = 1,
	};
	ENUM_CLASS_FLAGS(CameraProjection);

    CLASS()
	class SYSTEM_API CameraComponent : public SceneComponent
	{
		CE_CLASS(CameraComponent, SceneComponent)
	public:

		CameraComponent();

		inline const Color& GetClearColor() const { return clearColor; }

		inline void SetClearColor(const Color& clearColor) { this->clearColor = clearColor; }

		inline bool IsMainCamera() const { return isMainCamera; }

		void SetMainCamera(bool set = true);

		inline CameraProjection GetProjection() const { return projection; }
		inline void SetProjection(CameraProjection projection) { this->projection = projection; }

	private:

		FIELD()
		b8 isMainCamera = false;

		FIELD()
		Color clearColor = Color::RGBA(36, 85, 163);

		FIELD()
		CameraProjection projection{};

		FIELD()
		float nearPlane = 0.1f;

		FIELD()
		float farPlane = 1000;

		FIELD()
		float fieldOfView = 60;

		FIELD(ReadOnly)
		Matrix4x4 projectionMatrix{};

		friend class RendererSubsystem;
	};

} // namespace CE

#include "CameraComponent.rtti.h"