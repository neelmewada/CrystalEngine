#pragma once

namespace CE::RPI
{
	class Material;

	struct VertexBufferInfo
	{
		RHI::ShaderSemantic semantic{};
		u32 bufferIndex = 0;
		u32 byteOffset = 0;
		u32 byteCount = 0;
		u32 stride = 0;
		RHI::VertexAttributeDataType attributeType{};

		inline bool IsValid() const
		{
			return semantic.attribute != RHI::VertexInputAttribute::None;
		}
	};

	using VertexBufferList = FixedArray<VertexBufferInfo, RHI::Limits::Pipeline::MaxVertexAttribCount>;

	struct Mesh final
	{
		RHI::DrawArguments drawArguments{};
		RHI::IndexBufferView indexBufferView{};

		VertexBufferList vertexBufferInfos{};

		MaterialSlotId materialSlotId = 0;
	};

	class CORERPI_API ModelLod final
	{
	public:

		ModelLod();
		virtual ~ModelLod();

		static ModelLod* CreateCubeModel();

		static ModelLod* CreateSphereModel();

		void AddMesh(const Mesh& mesh);

		inline u32 GetMeshCount() const { return meshes.GetSize(); }

		inline Mesh* GetMesh(u32 index)
		{
			if (index >= meshes.GetSize())
				return nullptr;
			return &meshes[index]; 
		}

		inline RHI::Buffer* GetBuffer(u32 index) const { return trackedBuffers[index]; }

		//! @brief Always add index buffers at the very end!
		void TrackBuffer(RHI::Buffer* buffer);

		// Temporary functions!
		void BuildVertexInputSlotDescriptorList(u32 meshIndex, Array<RHI::VertexInputSlotDescriptor>& outInputSlots);
		void BuildVertexInputAttributeList(u32 meshIndex, Array<RHI::VertexAttributeDescriptor>& outVertexAttribs);

	private:

		// Each model can have multiple meshes (aka SubMeshes)
		Array<Mesh> meshes{};

		Array<RHI::Buffer*> trackedBuffers{};

		u32 totalVertexBuffers = 0;
		FixedArray<u8*, RHI::Limits::Pipeline::MaxVertexAttribCount> vertexDatas{};

	};
    
} // namespace CE::RPI

