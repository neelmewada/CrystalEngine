#pragma once

namespace CE
{
    ENUM()
    enum class VertexInputType
    {
        None = 0,
        Position,
        UV0,
        Normal,
        Tangent,
        Color,
    };
    ENUM_CLASS_FLAGS(VertexInputType);

    SYSTEM_API SIZE_T GetVertexInputTypeSize(VertexInputType input);

	STRUCT()
	struct SYSTEM_API SubMesh
	{
		CE_STRUCT(SubMesh)
	public:

		FIELD()
		s32 materialSlotIndex = 0;

		FIELD()
		Array<u32> indices{};

		void Clear();

		void Release();
	};

	STRUCT()
	struct SYSTEM_API Mesh
	{
		CE_STRUCT(Mesh)
	public:

		FIELD()
		Name name{};

		FIELD()
		Array<Vec3> vertices{};

		FIELD()
		Array<Vec2> uvCoords{};

		FIELD()
		Array<Vec3> normals{};

		FIELD()
		Array<Vec4> tangents{};

		FIELD()
		Array<Color> vertexColors{};

		FIELD()
		Array<SubMesh> submeshes{};

		FORCE_INLINE bool IsValid() const
		{
			return vertices.NonEmpty();
		}
        
        FORCE_INLINE u32 GetSubMeshCount() const
        {
            return submeshes.GetSize();
        }

		void Clear();

		void Release();
        
        RHI::Buffer* CreateBuffer(const Array<VertexInputType>& inputs = { VertexInputType::Position, VertexInputType::UV0, VertexInputType::Normal });
        
        void PushToBuffer(RHI::Buffer* buffer, const Array<VertexInputType>& inputs = { VertexInputType::Position, VertexInputType::UV0, VertexInputType::Normal });
	};

	CLASS()
	class SYSTEM_API ModelData : public Object
	{
		CE_CLASS(ModelData, Object)
	public:

		FIELD()
		Array<Mesh> lod{};

		static ModelData* GetCubeModel();
		
	};
    
} // namespace CE

#include "ModelData.rtti.h"
