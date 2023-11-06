#pragma once

namespace CE
{
    ENUM(Flags)
    enum class VertexInputAttribute
    {
        None = 0,
        Position = BIT(0),
        UV0 = BIT(1),
        Normal = BIT(2),
        Tangent = BIT(3),
        Color = BIT(4),
    };
    ENUM_CLASS_FLAGS(VertexInputAttribute);

    SYSTEM_API SIZE_T GetVertexInputTypeSize(VertexInputAttribute input);

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
		Array<SubMesh> subMeshes{};

		FORCE_INLINE bool IsValid() const
		{
			return vertices.NonEmpty();
		}
        
        FORCE_INLINE u32 GetSubMeshCount() const
        {
            return subMeshes.GetSize();
        }

		void Clear();
        
        RHI::Buffer* GetOrCreateBuffer(const Array<VertexInputAttribute>& inputs = { VertexInputAttribute::Position, VertexInputAttribute::UV0, VertexInputAttribute::Normal });
        
        void PushToBuffer(RHI::Buffer* buffer, const Array<VertexInputAttribute>& inputs = { VertexInputAttribute::Position, VertexInputAttribute::UV0, VertexInputAttribute::Normal });
		
		RHI::Buffer* GetOrCreateIndexBuffer();
        
        HashMap<VertexInputAttribute, RHI::Buffer*> vertexBuffers{};
		RHI::Buffer* indexBuffer = nullptr;

		b8 uses32BitIndex = false;
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
