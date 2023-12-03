#pragma once

namespace CE::RPI
{

	class CORERPI_API ShaderVariant final
	{
	public:

		ShaderVariant(const ShaderReflection& reflectionInfo);
		~ShaderVariant();

	private:

		SIZE_T variantId = 0;
		ShaderReflection reflectionInfo{};
		
		HashMap<RHI::ShaderStage, RHI::ShaderModule*> modulesByStage{};

		friend class Shader;
	};
    
} // namespace CE::RPI
