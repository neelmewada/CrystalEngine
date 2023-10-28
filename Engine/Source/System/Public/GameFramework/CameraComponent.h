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

	protected:

		FIELD()
		b8 isMainCamera = false;

		FIELD()
		Color clearColor = Color::RGBA(36, 85, 163);

		FIELD()
		CameraProjection projection{};
	};

} // namespace CE

#include "CameraComponent.rtti.h"