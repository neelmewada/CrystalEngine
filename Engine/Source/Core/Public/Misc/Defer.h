#pragma once

#define defer(action) CE::DeferImpl CE_CONCATENATE(__defer_, __LINE__)([&] { action; })

namespace CE
{
	class CORE_API DeferImpl
	{
	public:
		using Action = std::function<void(void)>;

		DeferImpl(const Action& act)
			: action(act) {}
		DeferImpl(Action&& act)
			: action(std::move(act)) {}

		DeferImpl(const DeferImpl& act) = delete;
		DeferImpl& operator=(const DeferImpl& act) = delete;

		DeferImpl(DeferImpl&& act) = delete;
		DeferImpl& operator=(DeferImpl&& act) = delete;

		~DeferImpl()
		{
			action();
		}

	private:
		Action action;

	};
}


