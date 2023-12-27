#pragma once

namespace CE::RPI
{
	class RenderPipeline;

	STRUCT()
	struct CORERPI_API RenderPipelineDescriptor
	{
		CE_STRUCT(RenderPipelineDescriptor)
	public:

		RenderPipelineDescriptor() = default;

		FIELD()
		Name name = "DefaultPipeline";

		FIELD()
		Name mainViewTag = "Camera";

		FIELD()
		PassRequest rootPass{};

		FIELD()
		Array<PassDefinition> passDefinitions{};

		/// @brief Output render target for this pipeline.
		RHI::RenderTarget* renderTarget = nullptr;

		PassDefinition* FindPassDefinition(const Name& passDefinition);

		PassRequest* FindPassRequest(const Name& passName, PassRequest* parent = nullptr);

		PassDefinition* FindPassDefinitionForPassRequest(const Name& passName);

	};

	struct CORERPI_API RenderPipelineBuilder
	{
	public:
		typedef RenderPipelineBuilder Self;

		RenderPipeline* Build();

		inline Self& WithName(const Name& name)
		{
			descriptor.name = name;
			return *this;
		}

		inline Self& SetRenderTarget(RHI::RenderTarget* renderTarget)
		{
			descriptor.renderTarget = renderTarget;
			return *this;
		}

		inline const RenderPipelineDescriptor& GetDescriptor() const 
		{ 
			return descriptor; 
		}

	private:
		friend class RenderPipeline;

		RenderPipelineDescriptor descriptor{};
	};
    
} // namespace CE::RPI

#include "RenderPipelineBuilder.rtti.h"
