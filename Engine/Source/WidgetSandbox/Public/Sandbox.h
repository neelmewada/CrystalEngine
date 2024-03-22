#pragma once

using namespace CE::Widgets;

namespace CE
{
	constexpr u32 MaxDirectionalLightCount = 8;

	struct alignas(16) PerViewData
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
		Vec4 viewPosition;
	};

	struct alignas(16) DirectionalLight
	{
		Matrix4x4 lightSpaceMatrix;
		Vec3 direction;
		Vec4 colorAndIntensity;
		float temperature;
	};

	struct alignas(16) PointLight
	{
		Vec3 position;
		Vec4 colorAndIntensity;
		float radius;
		float attenuation;
	};

	struct SceneConstants
	{
		f32 timeElapsed;
	};

	struct alignas(16) LightData
	{
		Vec4 ambientColor{};
		u32 totalDirectionalLights;
		u32 totalPointLights;
	};

	struct alignas(16) CameraData
	{
		Vec3 cameraPosition = {};
		f32 fieldOfView = 60;
	};

	class WidgetSandbox : ApplicationMessageHandler
	{
	public:

		WidgetSandbox() = default;

		void Init(PlatformWindow* window);

		void Tick(f32 deltaTime);

		void Shutdown();

	private:

		void InitFontAtlas();

		void InitWidgets();
		void DestroyWidgets();

		void BuildFrameGraph();
		void CompileFrameGraph();

		void SubmitWork(u32 imageIndex);

		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;

		void OnWindowDestroyed(PlatformWindow* window) override;

		void OnWindowMinimized(PlatformWindow* window) override;

		void OnWindowRestored(PlatformWindow* window) override;

		void OnWindowCreated(PlatformWindow* window) override;

	private:
		bool rebuild = true;
		bool recompile = true;
		bool resubmit = true;

		// Widgets
		// Root windows that represent a native PlatformWindow directly
		Array<CWindow*> rootWindows{};
		//Array<CWindow*> widgetWindows{};

		CDockSpace* mainDockSpace = nullptr;
		CDockWindow* mainDockWindow = nullptr;
		CDockWindow* secondDockWindow = nullptr;
		CDockWindow* thirdDockWindow = nullptr;
		CDockWindow* fourthDockWindow = nullptr;

		CDockSpace* secondDockSpace = nullptr;

		RHI::FrameScheduler* scheduler = nullptr;
		//RHI::SwapChain* swapChain = nullptr;
		//RHI::SwapChain* swapChain2 = nullptr;
		PlatformWindow* mainWindow = nullptr;

		Array<PlatformWindow*> platformWindows{};

		RPI::Model* chairModel = nullptr;

		RHI::DrawListContext drawList{};

		u32 width = 0;
		u32 height = 0;

		u32 width2 = 0;
		u32 height2 = 0;
		bool destroyed = false;

		int localCounter = 0;

		friend class SandboxLoop;
	};

} // namespace CE
