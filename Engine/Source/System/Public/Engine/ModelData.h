#pragma once

namespace CE
{
    ENUM()
    enum class VertexInputType
    {
        None = 0,
        Position = BIT(0),
        UV0 = BIT(1),
        Normal = BIT(2),
        Tangent = BIT(3),
        Color = BIT(4),
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

	CLASS()
	class SYSTEM_API Mesh : public Object
	{
		CE_CLASS(Mesh, Object)
	public:
        
        virtual ~Mesh();

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
        
        RHI::Buffer* CreateBuffer(const Array<VertexInputType>& inputs = { VertexInputType::Position, VertexInputType::UV0, VertexInputType::Normal });
        
        void PushToBuffer(RHI::Buffer* buffer, const Array<VertexInputType>& inputs = { VertexInputType::Position, VertexInputType::UV0, VertexInputType::Normal });
        
        
        HashMap<VertexInputType, RHI::Buffer*> vertexBuffers{};
	};

	CLASS()
	class SYSTEM_API ModelData : public Object
	{
		CE_CLASS(ModelData, Object)
	public:
        
        virtual ~ModelData();

		FIELD()
		Array<Mesh*> lod{};

		static ModelData* GetCubeModel();
		
    private:
        
        
	};
    
} // namespace CE

#include "ModelData.rtti.h"
