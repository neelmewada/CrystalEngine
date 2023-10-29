#pragma once

#define defer(x) CE::DeferImpl CE_CONCATENATE(__defer_, __LINE__) = [&]{ x; }

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

		template<class Func>
		DeferImpl(Func&& lamda) : action(std::move(lamda)) {}

		template<class Func>
		DeferImpl(const Func& lamda) : action(std::move(lamda)) {}

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


