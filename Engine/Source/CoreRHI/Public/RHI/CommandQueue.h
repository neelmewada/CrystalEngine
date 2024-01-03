#pragma once

namespace CE::RHI
{

	enum class HardwareQueueClass
	{
		Graphics = 0,

		Compute,

		//! Copy operations.
		Transfer,

		COUNT
	};
	ENUM_CLASS(HardwareQueueClass);

	constexpr u32 HardwareQueueClassCount = static_cast<u32>(HardwareQueueClass::COUNT);

	enum class HardwareQueueClassMask
	{
		None = 0,
		Graphics = BIT(static_cast<u32>(HardwareQueueClass::Graphics)),
		Compute = BIT(static_cast<u32>(HardwareQueueClass::Compute)),
		Transfer = BIT(static_cast<u32>(HardwareQueueClass::Transfer)),
		All = Graphics | Compute | Transfer
	};
	ENUM_CLASS_FLAGS(HardwareQueueClassMask);

	class CORERHI_API CommandQueue : RHIResource
	{
	protected:
		CommandQueue() : RHIResource(ResourceType::CommandQueue)
		{}

	public:

		inline HardwareQueueClassMask GetQueueMask() const
		{
			return queueMask;
		}

		inline bool SupportsOperation(HardwareQueueClass operationType) const
		{
			return (queueMask & (1 << (u32)operationType)) != 0;
		}

	protected:

		HardwareQueueClassMask queueMask{};

	};
    
} // namespace CE::RHI
