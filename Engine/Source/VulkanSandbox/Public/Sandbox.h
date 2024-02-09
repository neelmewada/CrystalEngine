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

	struct MaterialTextureGroup
	{
		Name name{};
		RHI::Texture* albedo = nullptr;
		RHI::Texture* normalMap = nullptr;
		RHI::Texture* roughnessMap = nullptr;
		RHI::MemoryHeap* memoryAllocation = nullptr;

		static MaterialTextureGroup Load(const Name& pathName);

		void Release();
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
		void InitTextures();
		void InitPipelines();
		void InitDrawPackets();
		void InitLights();

		// DrawPackets
		void BuildCubeMeshDrawPacket();
		void BuildSphereMeshDrawPacket();
		void BuildSkyboxDrawPacket();

		void DestroyLights();
		void DestroyDrawPackets();
		void DestroyPipelines();
		void DestroyTextures();
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

		MaterialTextureGroup woodFloorTextures{};
		MaterialTextureGroup plasticTextures{};

		RHI::ShaderResourceGroup* depthPerViewSrg = nullptr;
		RHI::ShaderResourceGroup* perViewSrg = nullptr;
		RHI::Buffer* perViewBuffer = nullptr;
		PerViewData perViewData{};

		RHI::PipelineState* depthPipeline = nullptr;
		RHI::ShaderModule* depthShaderVert = nullptr;
		
		RPI::Shader* opaqueShader = nullptr;
		RPI::Material* sphereMaterial = nullptr;
		RPI::Material* cubeMaterial = nullptr;
        RHI::Sampler* defaultSampler = nullptr;
		
		RPI::Shader* skyboxShader = nullptr;
		RPI::Material* skyboxMaterial = nullptr;

		RHI::PipelineState* transparentPipeline = nullptr;

		RHI::FrameScheduler* scheduler = nullptr;
		RHI::SwapChain* swapChain = nullptr;
		PlatformWindow* mainWindow = nullptr;

		RPI::ModelLod* cubeModel = nullptr;
		RPI::ModelLod* sphereModel = nullptr;
		RHI::DrawListContext drawList{};

		RHI::DrawPacket* sphereDrawPacket = nullptr;
		RHI::DrawPacket* cubeDrawPacket = nullptr;
		RHI::DrawPacket* skyboxDrawPacket = nullptr;

		RHI::ShaderResourceGroupLayout perSceneSrgLayout{};
		SceneConstants sceneConstants{};
		RHI::Buffer* sceneConstantBuffer = nullptr;

		Vec3 skyboxScale = Vec3(1000, 1000, 1000);
		float skyboxRotation = 0;
		RHI::ShaderResourceGroup* skyboxObjectSrg = nullptr;
		RHI::Buffer* skyboxObjectBuffer = nullptr;
		RHI::Buffer* skyboxViewBuffer = nullptr;
		Matrix4x4 skyboxModelMatrix{};
		PerViewData skyboxViewData{};

		RHI::ShaderResourceGroup* sphereObjectSrg = nullptr;
		RHI::Buffer* sphereObjectBuffer = nullptr;
		Matrix4x4 sphereModelMatrix{};
		f32 cameraRotation = 0.0f;
		f32 meshRotation = 0;
		
		RHI::ShaderResourceGroup* cubeObjectSrg = nullptr;
		RHI::Buffer* cubeObjectBuffer = nullptr;
		Matrix4x4 cubeModelMatrix{};

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
