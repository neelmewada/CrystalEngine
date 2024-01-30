#pragma once

namespace CE::RPI
{
	struct VertexBufferInfo
	{
		RHI::ShaderSemantic semantic{};
		u32 bufferIndex = 0;

		u64 byteOffset = 0;
		u64 byteCount = 0;

		u64 stride = 0;
	};

	using VertexBufferList = FixedArray<VertexBufferInfo, RHI::Limits::Pipeline::MaxVertexAttribCount>;

	struct Mesh final
	{
		RHI::DrawArguments drawArguments{};
		RHI::IndexBufferView indexBufferView{};

		VertexBufferList vertexBuffers{};

	};

	class CORERPI_API ModelLod final
	{
	public:

		ModelLod();
		virtual ~ModelLod();

		void AddMesh(const Mesh& mesh);

	private:

		// Each model can have multiple meshes (aka SubMeshes)
		Array<Mesh> meshes{};

		Array<RHI::Buffer*> vertexBuffers{};
		Array<RHI::Buffer*> indexBuffers{};

	};
    
} // namespace CE::RPI

