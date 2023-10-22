#pragma once

namespace CE
{
    CLASS()
	class SANDBOX_API SandboxGameInstance : public GameInstance
	{
		CE_CLASS(SandboxGameInstance, GameInstance)
	public:

		SandboxGameInstance();

	protected:

		void Initialize() override;
		void Shutdown() override;

	};

} // namespace CE

#include "SandboxGameInstance.rtti.h"