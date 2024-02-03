#include "CoreRPI.h"

namespace CE::RPI
{

	ModelLod::ModelLod()
	{

	}

	ModelLod::~ModelLod()
	{
		for (auto buffer : trackedBuffers)
		{
			delete buffer;
		}
		trackedBuffers.Clear();
	}

	ModelLod* ModelLod::CreateCubeModel()
	{
		static const Vec3 vertices[] = {
			Vec3(0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, 0.5),
			Vec3(0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),

			Vec3(0.5, 0.5, -0.5),
			Vec3(-0.5, 0.5, -0.5),
			Vec3(0.5, -0.5, -0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),
			Vec3(0.5, 0.5, -0.5),
			Vec3(-0.5, 0.5, -0.5),

			Vec3(0.5, -0.5, -0.5),
			Vec3(0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, 0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(-0.5, -0.5, 0.5),
			Vec3(-0.5, 0.5, 0.5),
			Vec3(-0.5, 0.5, -0.5),
			Vec3(-0.5, -0.5, -0.5),

			Vec3(0.5, -0.5, -0.5),
			Vec3(0.5, 0.5, -0.5),
			Vec3(0.5, 0.5, 0.5),
			Vec3(0.5, -0.5, 0.5)
		};

		static const Vec3 normals[] = {
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),
			Vec3(0, 0, 1),

			Vec3(0, 1, 0),
			Vec3(0, 1, 0),
			Vec3(0, 0, -1),
			Vec3(0, 0, -1),

			Vec3(0, 1, 0),
			Vec3(0, 1, 0),
			Vec3(0, 0, -1),
			Vec3(0, 0, -1),

			Vec3(0, -1, 0),
			Vec3(0, -1, 0),
			Vec3(0, -1, 0),
			Vec3(0, -1, 0),

			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),
			Vec3(-1, 0, 0),

			Vec3(1, 0, 0),
			Vec3(1, 0, 0),
			Vec3(1, 0, 0),
			Vec3(1, 0, 0)
		};

		static const Vec4 tangents[] = {
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),
			Vec4(-1, 0, 0, -1),

			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),
			Vec4(0, 0, -1, -1),

			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
			Vec4(0, 0, 1, -1),
		};

		static const Vec2 uvCoords[] = {
			Vec2(0, 0),
			Vec2(1, 0),
			Vec2(0, 1),
			Vec2(1, 1),

			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(0, 1),
			Vec2(1, 1),

			Vec2(0, 0),
			Vec2(1, 0),
			Vec2(0, 0),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0),

			Vec2(0, 0),
			Vec2(0, 1),
			Vec2(1, 1),
			Vec2(1, 0)
		};

		static const u16 indices[] = {
			0, 2, 3,
			0, 3, 1,
			8, 4, 5,
			8, 5, 9,
			10, 6, 7,
			10, 7, 11,
			12, 13, 14,
			12, 14, 15,
			16, 17, 18,
			16, 18, 19,
			20, 21, 22,
			20, 22, 23
		};
		
		constexpr u64 totalBufferSize = sizeof(vertices) + sizeof(normals) + sizeof(uvCoords) + sizeof(tangents) + sizeof(indices);

		ModelLod* model = new ModelLod();
		
		RHI::BufferDescriptor bufferDesc{};
		bufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer | RHI::BufferBindFlags::IndexBuffer;
		bufferDesc.bufferSize = totalBufferSize;
		bufferDesc.defaultHeapType = RHI::MemoryHeapType::Default;
		bufferDesc.name = "Cube Mesh Buffer";
		
		RHI::Buffer* buffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);

		model->TrackBuffer(buffer);

		Mesh mesh{};
		mesh.indexBufferView = RHI::IndexBufferView(buffer, totalBufferSize - sizeof(indices), sizeof(indices), RHI::IndexFormat::Uint16);

		RHI::DrawIndexedArguments drawArgs{};
		drawArgs.firstIndex = 0;
		drawArgs.indexCount = COUNTOF(indices);
		drawArgs.firstInstance = 0;
		drawArgs.instanceCount = 1;
		drawArgs.vertexOffset = 0;
		mesh.drawArguments = RHI::DrawArguments(drawArgs);

		u32 offset = 0;
		
		{
			VertexBufferInfo vertInfo{};
			vertInfo.attributeType = RHI::VertexAttributeDataType::Float3;
			vertInfo.bufferIndex = 0;
			vertInfo.byteOffset = offset;
			vertInfo.byteCount = sizeof(vertices);
			vertInfo.stride = sizeof(Vec3);
			
			vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Position);
			mesh.vertexBufferInfos.Add(vertInfo);

			buffer->UploadData(vertices, vertInfo.byteCount, vertInfo.byteOffset);
			offset += vertInfo.byteCount;
		}

		{
			VertexBufferInfo vertInfo{};
			vertInfo.attributeType = RHI::VertexAttributeDataType::Float3;
			vertInfo.bufferIndex = 0;
			vertInfo.byteOffset = offset;
			vertInfo.byteCount = sizeof(normals);
			vertInfo.stride = sizeof(Vec3);

			vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Normal);
			mesh.vertexBufferInfos.Add(vertInfo);

			buffer->UploadData(normals, vertInfo.byteCount, vertInfo.byteOffset);
			offset += vertInfo.byteCount;
		}

		{
			VertexBufferInfo vertInfo{};
			vertInfo.attributeType = RHI::VertexAttributeDataType::Float2;
			vertInfo.bufferIndex = 0;
			vertInfo.byteOffset = offset;
			vertInfo.byteCount = sizeof(uvCoords);
			vertInfo.stride = sizeof(Vec2);

			vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::UV, 0);
			mesh.vertexBufferInfos.Add(vertInfo);

			buffer->UploadData(uvCoords, vertInfo.byteCount, vertInfo.byteOffset);
			offset += vertInfo.byteCount;
		}
		
		{
			VertexBufferInfo vertInfo{};
			vertInfo.attributeType = RHI::VertexAttributeDataType::Float4;
			vertInfo.bufferIndex = 0;
			vertInfo.byteOffset = offset;
			vertInfo.byteCount = sizeof(tangents);
			vertInfo.stride = sizeof(Vec4);

			vertInfo.semantic = RHI::ShaderSemantic(RHI::VertexInputAttribute::Tangent);
			mesh.vertexBufferInfos.Add(vertInfo);

			buffer->UploadData(tangents, vertInfo.byteCount, vertInfo.byteOffset);
			offset += vertInfo.byteCount;
		}

		// Index Data
		{
			buffer->UploadData(indices, sizeof(indices), offset);
			offset += sizeof(indices);
		}

		mesh.materialSlotId = 0;

		model->AddMesh(mesh);

		return model;
	}

	void ModelLod::AddMesh(const Mesh& mesh)
	{
		RHI::Buffer* buffer = mesh.indexBufferView.GetBuffer();
		if (buffer == nullptr)
			return;

		for (const VertexBufferInfo& vertexBufferInfo : mesh.vertexBufferInfos)
		{
			totalVertexBuffers = Math::Max(totalVertexBuffers, vertexBufferInfo.bufferIndex + 1);
		}

		meshes.Add(mesh);
	}

	void ModelLod::TrackBuffer(RHI::Buffer* buffer)
	{
		if (!trackedBuffers.Exists(buffer))
		{
			trackedBuffers.Add(buffer);
		}
	}

	void ModelLod::BuildVertexInputSlotDescriptorList(u32 meshIndex, Array<RHI::VertexInputSlotDescriptor>& inputSlots)
	{
		inputSlots.Clear();
		if (meshIndex >= meshes.GetSize())
			return;
		
		const Mesh& mesh = meshes[meshIndex];
		inputSlots.Resize(mesh.vertexBufferInfos.GetSize());
		int slotIndex = 0;

		for (const auto& vertexBufferInfo : mesh.vertexBufferInfos)
		{
			inputSlots[slotIndex].inputRate = RHI::VertexInputRate::PerVertex;
			inputSlots[slotIndex].inputSlot = slotIndex;
			inputSlots[slotIndex].stride = vertexBufferInfo.stride;

			slotIndex++;
		}
	}

	void ModelLod::BuildVertexInputAttributeList(u32 meshIndex, Array<RHI::VertexAttributeDescriptor>& vertexAttribs)
	{
		vertexAttribs.Clear();
		if (meshIndex >= meshes.GetSize())
			return;
		
		const Mesh& mesh = meshes[meshIndex];
		vertexAttribs.Resize(mesh.vertexBufferInfos.GetSize());
		int slotIndex = 0;
		
		for (const auto& vertexBufferInfo : mesh.vertexBufferInfos)
		{
			vertexAttribs[slotIndex].dataType = vertexBufferInfo.attributeType;
			vertexAttribs[slotIndex].inputSlot = slotIndex;
			vertexAttribs[slotIndex].location = slotIndex;
			vertexAttribs[slotIndex].offset = 0;
			
			slotIndex++;
		}
	}

} // namespace CE::RPI
