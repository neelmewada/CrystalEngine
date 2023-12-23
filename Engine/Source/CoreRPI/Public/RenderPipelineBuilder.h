#pragma once

namespace CE::RPI
{
	class RenderPipeline;

	STRUCT()
	struct CORERPI_API PassData
	{
		CE_STRUCT(PassData)
	public:

		FIELD()
		Name drawListTag{};

		FIELD()
		Name viewTag{};

	};

	/// @brief Describes the pass to create as a subpass of current pass.
	STRUCT()
	struct CORERPI_API PassRequest
	{
		CE_STRUCT(PassRequest)
	public:

		/// @brief Name of the pass after instantiation
		FIELD()
		Name passName{};

		/// @brief Name of pass template we will instantiate from
		FIELD()
		Name passDefinition{};

		/// @brief Connections of the instantiated pass
		FIELD()
		Array<PassConnection> connections{};

	};

	STRUCT()
	struct CORERPI_API PassDefinition
	{
		CE_STRUCT(PassDefinition)
	public:

		FIELD()
		Name name{};

		FIELD()
		Name passClass{};

		FIELD()
		Array<PassSlot> slots{};

		FIELD()
		Array<PassConnection> connections{};

		FIELD()
		Array<PassImageAttachmentDesc> imageAttachments{};

		FIELD()
		PassData passData{};
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
		Name mainViewTag = "Camera";

		FIELD()
		PassRequest rootPass{};

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
