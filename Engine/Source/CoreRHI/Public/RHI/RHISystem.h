#pragma once

namespace CE::RHI
{
    
	class CORERHI_API RHISystem final
	{
	public:

		RHISystem() = default;

		inline DrawListTagRegistry* GetDrawListTagRegistry()
		{
			return &listTagRegistry;
		}

	private:

		DrawListTagRegistry listTagRegistry{};

	};

} // namespace CE::RHI
