#pragma once

namespace CE::RHI
{

	class CORERHI_API Sampler : public Resource
	{
	protected:
		Sampler() : Resource(ResourceType::Sampler)
		{}

	public:

		virtual ~Sampler() = default;

		virtual void* GetHandle() = 0;

	};
    
} // namespace CE::RHI
