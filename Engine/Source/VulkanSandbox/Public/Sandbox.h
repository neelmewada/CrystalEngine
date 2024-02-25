#pragma once

namespace CE::Sandbox
{
	constexpr u32 MaxDirectionalLightCount = 8;



	struct alignas(16) PerViewData
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
		Vec3 viewPosition;
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

	struct MaterialTextureGroup
	{
		Name name{};
		CE::Texture* albedo = nullptr;
		CE::Texture* normalMap = nullptr;
		CE::Texture* roughnessMap = nullptr;
		CE::Texture* metallicMap = nullptr;

		static MaterialTextureGroup Load(const Name& pathName);

		void Release();
	};

	class VulkanSandbox : IWindowCallbacks
	{
	public:

		VulkanSandbox() = default;

		void Init(PlatformWindow* window);

		void Tick(f32 deltaTime);

		void UpdatePerViewData(int imageIndex);

		void Shutdown();
		
		void InitCubeMaps();
		void InitHDRIs();
        void InitIrradiancePipeline(const RHI::ShaderResourceGroupLayout& irradianceSrgLayout);
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
		void DestroyHDRIs();
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

		RHI::Texture* hdriMap = nullptr;
		RHI::Texture* hdriGrayscaleMap = nullptr;
		RHI::Texture* hdriRowAverage = nullptr;
		RHI::Texture* hdriColumnAverage = nullptr;
		RHI::Texture* hdriCDFMarginalInverse = nullptr;
		RHI::Texture* hdriPDFConditional = nullptr;
		RHI::Texture* hdriCDFConditionalInverse = nullptr;
		RHI::Texture* hdriPDFJoint = nullptr;
		RHI::Texture* hdriPDFMarginal = nullptr;
		RHI::Texture* hdriIrradiance = nullptr;

		RHI::Texture* hdriCubeMap = nullptr;
        RHI::Texture* irradianceMap = nullptr;
        RHI::PipelineState* irradiancePipeline = nullptr;
        RHI::ShaderModule* convolutionVertShader = nullptr;
        RHI::ShaderModule* convolutionFragShader = nullptr;
		RHI::ShaderModule* grayscaleVertShader = nullptr;
		RHI::ShaderModule* grayscaleFragShader = nullptr;

		RHI::Texture* skyboxCubeMap = nullptr;
		RHI::Sampler* skyboxSampler = nullptr;

		MaterialTextureGroup woodFloorTextures{};
		MaterialTextureGroup plasticTextures{};
		MaterialTextureGroup rustedTextures{};

		RHI::ShaderResourceGroup* depthPerViewSrg = nullptr;
		RHI::ShaderResourceGroup* perViewSrg = nullptr;
		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> perViewBufferPerImage{};
		PerViewData perViewData{};

		PerViewData directionalLightViewData{};
		RHI::ShaderResourceGroup* directionalLightViewSrg = nullptr;
		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> directionalLightViewPerImage{};
		RHI::Sampler* shadowMapSampler = nullptr;

		RHI::PipelineState* depthPipeline = nullptr;
		RHI::ShaderModule* depthShaderVert = nullptr;
		
		CE::Shader* opaqueShader = nullptr;
		RPI::Shader* opaqueRpiShader = nullptr;
		//RPI::Shader* opaqueShader = nullptr;
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
		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> skyboxObjectBufferPerImage{};
		Matrix4x4 skyboxModelMatrix{};
		PerViewData skyboxViewData{};

		RHI::ShaderResourceGroup* sphereObjectSrg = nullptr;
		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> sphereObjectBufferPerImage{};
		Matrix4x4 sphereModelMatrix{};
		f32 cameraRotation = 0.0f;
		f32 sphereRotation = 0;
        f32 cubeRotation = 0;

		RHI::ShaderResourceGroup* cubeObjectSrg = nullptr;
		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> cubeObjectBufferPerImage{};
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
