#pragma once

namespace CE
{
	class RenderPipeline;
	
	ENUM()
	enum class CameraProjection
	{
		Perspective = 0,
		Orthogonal = 1,
	};
	ENUM_CLASS(CameraProjection);

	ENUM()
	enum class CameraType
	{
		MainCamera = 0,
		SecondaryCamera
	};
	ENUM_CLASS(CameraType);

    CLASS()
	class SYSTEM_API CameraComponent : public SceneComponent
	{
		CE_CLASS(CameraComponent, SceneComponent)
	public:

		CameraComponent();

		virtual ~CameraComponent();

		const Color& GetClearColor() const { return clearColor; }

		void SetClearColor(const Color& clearColor) { this->clearColor = clearColor; }

		CameraProjection GetProjection() const { return projection; }
		void SetProjection(CameraProjection projection) { this->projection = projection; }

		float GetFarPlane() const { return farPlane; }

    	float GetNearPlane() const { return nearPlane; }

		float GetFieldOfView() const { return fieldOfView; }

		RPI::View* GetRpiView() const { return rpiView; }
    	
    	void SetRenderPipeline(CE::RenderPipeline* renderPipeline);
    	
		CE::RenderPipeline* GetRenderPipeline() const { return renderPipeline; }

    	const Name& GetViewTag() const { return cameraType == CameraType::MainCamera ? "MainCamera" : viewTag; }

    	void SetViewTag(const Name& tag) { viewTag = tag; }

		CWindow* GetRenderWindow() const { return renderWindow; }
    	
    protected:

		void Tick(f32 delta) override;

	private:

    	FIELD()
    	Name viewTag = "MainCamera";

    	FIELD()
    	CameraType cameraType = CameraType::MainCamera;

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

		FIELD(ReadOnly)
		Matrix4x4 viewMatrix{};

		//! @brief The CWindow that this scene is rendered to.
		FIELD()
		CWindow* renderWindow = nullptr;

    	FIELD()
    	CE::RenderPipeline* renderPipeline = nullptr;

		RPI::ViewPtr rpiView = nullptr;

		friend class CE::Scene;
		friend class RendererSubsystem;
	};

} // namespace CE

#include "CameraComponent.rtti.h"