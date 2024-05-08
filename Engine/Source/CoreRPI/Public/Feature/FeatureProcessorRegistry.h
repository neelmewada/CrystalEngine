#pragma once

namespace CE::RPI
{
	class FeatureProcessor;
    
	class CORERPI_API FeatureProcessorRegistry final
	{
	private:
		FeatureProcessorRegistry() = default;
		CE_NO_COPY(FeatureProcessorRegistry);

	public:

		static FeatureProcessorRegistry& Get();

	private:

		void OnClassRegistered(ClassType* classType);
		void OnClassDeregistered(ClassType* classType);

		Array<SubClassType<FeatureProcessor>> featureProcessorClasses{};

		friend class CoreRPIModule;
	};

} // namespace CE::RPI
