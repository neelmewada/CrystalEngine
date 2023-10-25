#pragma once

namespace CE
{
	STRUCT()
	struct SYSTEM_API Mesh
	{
		CE_STRUCT(Mesh)
	public:

		FIELD()
		s32 materialSlotIndex = 0;

		FIELD()
		Name name{};

		FIELD()
		Array<Vec3> vertices{};

		FIELD()
		Array<Vec2> uvCoords{};

		FIELD()
		Array<Vec3> normals{};

		FIELD()
		Array<Vec3> tangents{};

		FIELD()
		Array<u32> indices{};

		FIELD()
		Array<Color> vertexColors{};

		FORCE_INLINE bool IsValid() const
		{
			return vertices.NonEmpty();
		}

		void Clear();

		void Release();
	};

	STRUCT()
	struct SYSTEM_API MeshLOD
	{
		CE_STRUCT(MeshLOD)
	public:

		FIELD()
		Array<Mesh> mesh{};
	};

	CLASS()
	class SYSTEM_API ModelData : public Object
	{
		CE_CLASS(ModelData, Object)
	public:

		FIELD()
		Array<MeshLOD> lod{};
		
	};
    
} // namespace CE

#include "ModelData.rtti.h"