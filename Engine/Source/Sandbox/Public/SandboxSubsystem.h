#pragma once

namespace CE
{
    CLASS()
	class SANDBOX_API SandboxSubsystem : public GameInstanceSubsystem
	{
		CE_CLASS(SandboxSubsystem, GameInstanceSubsystem)
	public:
		SandboxSubsystem();

	protected:

		void Initialize() override;
		void Shutdown() override;

		void Tick(f32 delta) override;
	};

} // namespace CE

#include "SandboxSubsystem.rtti.h"