#pragma once

namespace CE
{
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

		void Clear();

		void Release();
	};

	CLASS()
	class SYSTEM_API ModelData : public Object
	{
		CE_CLASS(ModelData, Object)
	public:

		FIELD()
		Array<Mesh> lod{};

		static ModelData* GetCubeMesh();
		
	};
    
} // namespace CE

#include "ModelData.rtti.h"