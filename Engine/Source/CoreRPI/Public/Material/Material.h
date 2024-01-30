#pragma once

namespace CE::RPI
{
	typedef HashMap<Name, MaterialPropertyValue> MaterialPropertyValueMap;

	class CORERPI_API Material
	{
	public:

		Material();
		~Material();

	private:
		
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		MaterialPropertyValueMap properties{};

	};
    
} // namespace CE::RPI
