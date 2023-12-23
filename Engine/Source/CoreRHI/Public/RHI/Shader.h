#pragma once

namespace CE::RHI
{

	class CORERHI_API ShaderModule : public RHIResource
	{
	protected:
		ShaderModule() : RHIResource(ResourceType::ShaderModule)
		{}

	public:
		virtual ~ShaderModule() = default;

		// - Public API -

		virtual Name GetName() = 0;

		virtual bool IsValidShader() = 0;

		virtual ShaderStage GetShaderStage() = 0;

		bool IsVertexShader() { return GetShaderStage() == ShaderStage::Vertex; }
		bool IsFragmentShader() { return GetShaderStage() == ShaderStage::Fragment; }

	};
    
} // namespace CE::RHI
