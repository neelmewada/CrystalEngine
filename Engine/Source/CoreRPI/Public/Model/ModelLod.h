#pragma once

namespace CE::RPI
{

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
		s32 materialSlotIndex = 0;

		FIELD()
		Array<u32> indices{};

		FORCE_INLINE bool IsValid() const
		{
			return vertices.NonEmpty();
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

