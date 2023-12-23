#pragma once

namespace CE::RPI
{
	class RenderPipeline;

	STRUCT()
	struct CORERPI_API PassDefinition
	{
		CE_STRUCT(PassDefinition)
	public:

		FIELD()
		Name name{};


	};

	STRUCT()
	struct CORERPI_API RenderPipelineDescriptor
	{
		CE_STRUCT(RenderPipelineDescriptor)
	public:

		RenderPipelineDescriptor() = default;

		FIELD()
		Name name = "DefaultPipeline";

		FIELD()
		Array<String> viewTags{};

		FIELD()
		Name rootPass = "DefaultPipelineRoot";

		FIELD()
		Array<PassDefinition> passDefinitions{};

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

		inline const RenderPipelineDescriptor& GetDescriptor() const 
		{ 
			return descriptor; 
		}

	private:

		RenderPipelineDescriptor descriptor{};
	};
    
} // namespace CE::RPI

#include "RenderPipelineBuilder.rtti.h"
