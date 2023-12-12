#pragma once

namespace CE::RPI
{

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

