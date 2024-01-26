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
	};

	class VulkanSandbox : IWindowCallbacks
	{
	public:

		VulkanSandbox() = default;

		void Init(PlatformWindow* window);

		void Tick(f32 deltaTime);

		void Shutdown();
		
		void InitPipelines();
		void InitModels();

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

		RHI::ShaderResourceGroup* perViewSrg = nullptr;
		RHI::Buffer* perViewBuffer = nullptr;
		PerViewData perViewData{};

		RHI::PipelineState* depthPipeline = nullptr;
		RHI::ShaderModule* depthShaderVert = nullptr;

		RHI::PipelineState* opaquePipeline = nullptr;
		RHI::ShaderModule* opaqueShaderVert = nullptr;
		RHI::ShaderModule* opaqueShaderFrag = nullptr;

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

		u32 width = 0;
		u32 height = 0;
		bool destroyed = false;

		int localCounter = 0;

		friend class SandboxLoop;
	};

} // namespace CE
