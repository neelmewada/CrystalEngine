#pragma once

namespace CE::RPI
{
	class PassTemplate;
	struct PassRequest;
	class Pass;

	struct PassDesc
	{
		PassDesc() = default;
		~PassDesc() = default;

		/// @brief Name of the pass.
		Name passName{};

		/// @brief Optional: Pass template to use to create the pass.
		Ptr<const PassTemplate> passTemplate = nullptr;

		/// @brief Optional: PassRequest to use to create this pass.
		const PassRequest* passRequest = nullptr;
		
	};
    
} // namespace CE::RPI
