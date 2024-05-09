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

		void SetFarPlane(f32 value) { farPlane = value; }

    	float GetNearPlane() const { return nearPlane; }

		void SetNearPlane(f32 value) { nearPlane = value; }

		float GetFieldOfView() const { return fieldOfView; }

		RPI::View* GetRpiView() const { return rpiView; }

		Vec2 GetViewport() const { return viewport; }

		void SetViewport(const Vec2& value) { viewport = value; }
    	
    	void SetRenderPipeline(CE::RenderPipeline* renderPipeline);
    	
		CE::RenderPipeline* GetRenderPipeline() const { return renderPipeline; }

    	const Name& GetViewTag() const { return cameraType == CameraType::MainCamera ? "MainCamera" : viewTag; }

    	void SetViewTag(const Name& tag) { viewTag = tag; }
    	
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
    
		FIELD()
		Vec2 viewport = Vec2(1, 1);

		FIELD()
		Vec2i windowSize = Vec2i(0, 0);

    	FIELD()
    	CE::RenderPipeline* renderPipeline = nullptr;

		RPI::ViewPtr rpiView = nullptr;

		friend class CE::Scene;
		friend class RendererSubsystem;
	};

} // namespace CE

#include "CameraComponent.rtti.h"