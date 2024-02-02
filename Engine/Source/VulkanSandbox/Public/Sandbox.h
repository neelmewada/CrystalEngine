#pragma once

namespace CE::Sandbox
{
	struct VertexStruct
	{
		Vec3 position;
		Vec3 normal;
		Vec3 tangent;
		Vec2 uvCoord;
	};

	constexpr u32 MaxDirectionalLightCount = 8;

	class Mesh
	{
	public:

		constexpr static u64 VertexBufferStride = sizeof(VertexStruct);

		~Mesh()
		{
			if (buffer)
				RHI::gDynamicRHI->DestroyBuffer(buffer);
			buffer = nullptr;
		}

		void CreateBuffer();

		Array<Vec3> vertices{};

		Array<u32> indices{};

		Array<Vec2> uvCoords{};

		Array<Vec3> normals{};

		Array<Vec3> tangents{};

		RHI::Buffer* buffer = nullptr;
		u64 vertexBufferStride = 0;
		u64 vertexBufferOffset = 0;
		u64 vertexBufferSize = 0;
		u64 indexBufferOffset = 0;
		u64 indexBufferSize = 0;
	};

	struct MeshInstance
	{
		Mesh* mesh = nullptr;
		Vec3 position{};
		Quat rotation{};
		Vec3 scale{};
	};

	struct PerViewData
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
		Vec3 viewPosition;
	};

	struct DirectionalLight
	{
		Vec3 direction;
		Vec4 colorAndIntensity;
		float temperature;
	};

	struct PointLight
	{
		alignas(16) Vec3 position;
		alignas(16) Vec3 color;
		alignas(4)	float intensity;
		alignas(4)	float radius;
		alignas(4)	float attenuation;
	};

	struct LightData
	{
		alignas(16) Vec4 ambientColor{};
		alignas(4)  u32 totalDirectionalLights;
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
		
		void InitPipelines();
		void InitModels();
		void InitLights();

		void DestroyLights();
		void DestroyModels();
		void DestroyPipelines();

	private:

		void BuildFrameGraph();
		void CompileFrameGraph();

		void SubmitWork();

		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;

		void OnWindowDestroyed(PlatformWindow* window) override;

		bool rebuild = true;
		bool recompile = true;
		bool resubmit = true;

		RHI::ShaderResourceGroup* depthPerViewSrg = nullptr;
		RHI::ShaderResourceGroup* perViewSrg = nullptr;
		RHI::Buffer* perViewBuffer = nullptr;
		PerViewData perViewData{};

		RHI::PipelineState* depthPipeline = nullptr;
		RHI::ShaderModule* depthShaderVert = nullptr;

		RPI::Shader* opaqueShader = nullptr;
		RPI::Material* opaqueMaterial = nullptr;

		RHI::PipelineState* transparentPipeline = nullptr;

		RHI::FrameScheduler* scheduler = nullptr;
		RHI::SwapChain* swapChain = nullptr;
		PlatformWindow* mainWindow = nullptr;

		Array<Mesh*> meshes{};
		Array<MeshInstance> meshInstances{};
		RHI::DrawListContext drawList{};

		DrawPacket* meshDrawPacket = nullptr;

		RHI::ShaderResourceGroup* meshObjectSrg = nullptr;
		RHI::Buffer* meshModelBuffer = nullptr;
		Matrix4x4 meshModelMatrix{};
		f32 cameraRotation = 0.0f;
		f32 meshRotation = 0;

		RHI::ShaderResourceGroup* perSceneSrg = nullptr;
		RHI::Buffer* directionalLightsBuffer = nullptr;
		RHI::Buffer* lightDataBuffer = nullptr;
		FixedArray<DirectionalLight, MaxDirectionalLightCount> directionalLights{};
		LightData lightData{};

		u32 width = 0;
		u32 height = 0;
		bool destroyed = false;

		int localCounter = 0;

		friend class SandboxLoop;
	};

} // namespace CE
