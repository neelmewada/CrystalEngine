#pragma once

namespace CE::Sandbox
{
	constexpr u32 MaxDirectionalLightCount = 8;



	struct PerViewData
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
		Vec3 viewPosition;
	};

	struct alignas(16) DirectionalLight
	{
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

	struct LightData
	{
		Vec4 ambientColor{};
		u32 totalDirectionalLights;
		u32 totalPointLights;
	};

	struct CameraData
	{
		alignas(16) Vec3 cameraPosition = {};
		alignas(4)	f32 fieldOfView = 60;

	};

	class VulkanSandbox : IWindowCallbacks
	{
	public:

		VulkanSandbox() = default;

		void Init(PlatformWindow* window);

		void Tick(f32 deltaTime);

		void UpdatePerViewData();

		void Shutdown();
		
		void InitCubeMaps();
		void InitPipelines();
		void InitDrawPackets();
		void InitLights();

		void DestroyLights();
		void DestroyDrawPackets();
		void DestroyPipelines();
		void DestroyCubeMaps();

	private:

		void BuildFrameGraph();
		void CompileFrameGraph();

		void SubmitWork();

		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;

		void OnWindowDestroyed(PlatformWindow* window) override;

		bool rebuild = true;
		bool recompile = true;
		bool resubmit = true;

		RHI::Texture* skyboxCubeMap = nullptr;
		RHI::Sampler* skyboxSampler = nullptr;

		RHI::ShaderResourceGroup* depthPerViewSrg = nullptr;
		RHI::ShaderResourceGroup* perViewSrg = nullptr;
		RHI::Buffer* perViewBuffer = nullptr;
		PerViewData perViewData{};

		RHI::PipelineState* depthPipeline = nullptr;
		RHI::ShaderModule* depthShaderVert = nullptr;

		RPI::Shader* opaqueShader = nullptr;
		RPI::Material* opaqueMaterial = nullptr;
        RHI::Sampler* defaultSampler = nullptr;

		RHI::PipelineState* transparentPipeline = nullptr;

		RHI::FrameScheduler* scheduler = nullptr;
		RHI::SwapChain* swapChain = nullptr;
		PlatformWindow* mainWindow = nullptr;

		RPI::ModelLod* cubeModel = nullptr;
		RPI::ModelLod* sphereModel = nullptr;
		RHI::DrawListContext drawList{};

		DrawPacket* meshDrawPacket = nullptr;

		RHI::ShaderResourceGroup* cubeObjectSrg = nullptr;
		RHI::Buffer* cubeObjectBuffer = nullptr;
		Matrix4x4 meshModelMatrix{};
		f32 cameraRotation = 0.0f;
		f32 meshRotation = 0;

		RHI::ShaderResourceGroup* perSceneSrg = nullptr;
		RHI::Buffer* directionalLightsBuffer = nullptr;
		RHI::Buffer* pointLightsBuffer = nullptr;
		RHI::Buffer* lightDataBuffer = nullptr;
		FixedArray<DirectionalLight, MaxDirectionalLightCount> directionalLights{};
		Array<PointLight> pointLights{};
		LightData lightData{};

		u32 width = 0;
		u32 height = 0;
		bool destroyed = false;

		int localCounter = 0;

		friend class SandboxLoop;
	};

} // namespace CE
