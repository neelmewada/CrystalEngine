#pragma once

namespace CE::RHI
{

	class CORERHI_API Sampler : public RHIResource
	{
	protected:
		Sampler() : RHIResource(ResourceType::Sampler)
		{}

	public:

		virtual ~Sampler() = default;

		virtual void* GetHandle() = 0;

	};
    
} // namespace CE::RHI
