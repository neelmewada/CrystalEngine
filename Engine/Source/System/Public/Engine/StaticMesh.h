#pragma once

#include "ModelData.h"

namespace CE
{
	ENUM()
	enum class MeshSourceFormat
	{
		None = 0,
		FBX = 1,
		GLTF = 2,
	};
	ENUM_CLASS_FLAGS(MeshSourceFormat);

	STRUCT()
	struct SYSTEM_API StaticMeshSource
	{
		CE_STRUCT(StaticMeshSource)
	public:

		FIELD()
		BinaryBlob source{};

		FIELD()
		MeshSourceFormat format = MeshSourceFormat::None;
	};

    CLASS()
	class SYSTEM_API StaticMesh : public Asset
	{
		CE_CLASS(StaticMesh, Asset)
	public:

		StaticMesh();

		inline ModelData* GetModelData() const { return modelData; }

		inline void SetMesh(ModelData* modelData) { this->modelData = modelData; }

		static StaticMesh* GetCubeMesh();
        
        RHI::Buffer* GetVertexBuffer();

	protected:

		FIELD()
		StaticMeshSource source{};

		FIELD()
		ModelData* modelData = nullptr;
        
        
	};
	
} // namespace CE

#include "StaticMesh.rtti.h"
