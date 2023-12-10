#pragma once

namespace CE::RPI
{
	STRUCT()
	struct CORERPI_API SubMesh
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
	class CORERPI_API Mesh : public Object
	{
		CE_CLASS(Mesh, Object)
	public:

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
	};

	class CORERPI_API ModelLod final
	{
	public:

		ModelLod();
		virtual ~ModelLod();

		void TrackBuffer(RHI::Buffer* buffer);

	private:

		RHI::Buffer* vertexBuffer = nullptr;
		RHI::Buffer* indexBuffer = nullptr;

		Array<RHI::Buffer*> trackedBuffers{};

	};
    
} // namespace CE::RPI

#include "ModelLod.rtti.h"
